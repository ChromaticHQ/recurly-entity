<?php

/**
 * Queue adjustments for importing.
 *
 * Importing adjustments is a two-step process. First, we have to query for
 * invoices. From each invoice, an Adjustment is already returned fully-loaded.
 * However, saving an adjustment could trigger additional Recurly API requests,
 * or place a reasonable load on the local system as the entity system
 * processes and saves the adjustment. As well, each adjustment is independent
 * of one another, so by queuing them not only do we unblock the invoice call
 * quickly, but we can potentially process adjustments with multiple queue
 * runners on multiple servers.
 *
 * @param int $limit
 *   Optional parameter of the maximum number of invoices to process.
 */
function recurly_entity_adjustment_queue($limit = 50) {
  recurly_client_initialize();

  // We cache a copy of our pager to persist between requests. Otherwise, there
  // is no way to retrieve the state of the pager without rewriting it to store
  // it's state in public methods and variables.
  if ($cached = cache_get('recurly_entity_invoice_pager')) {
    $invoices = $cached->data;
    watchdog('recurly_entity', 'Loaded invoices from the local cache.', array(), WATCHDOG_DEBUG);
  }
  else {
    $invoices = Recurly_InvoiceList::get();
    watchdog('recurly_entity', 'Loaded invoices from Recurly.', array(), WATCHDOG_DEBUG);
  }

  // TODO: Use cache for this variable?
  $last_watermark = variable_get('recurly_entity_invoice_watermark', "");

  $imported = 0;
  // We can't use foreach() as that resets the current position when it starts.
  // Caching the pager object allows us to continue from the current position
  // in a given result set. Another concern is the size of the cached pager.
  // Recurly limits result sets to a maximum of 200 items, and Recurly_Pager
  // resets it's internal array of pagers on each result set.
  while ($invoices->valid()) {
    $invoice = $invoices->current();
    // Keep track of what the most recent UUID is from Recurly.
    if (!isset($first_invoice)) {
      if ($cached = cache_get('recurly_entity_invoice_first')) {
        $first_invoice = $cached->data;
      }
      else {
        $first_invoice = $invoice->uuid;
        cache_set('recurly_entity_invoice_first', $first_invoice);
      }
    }

    if (++$imported > $limit) {
      // Preserve our position for the next queue run.
      watchdog('recurly_entity', 'Reached import limit.', array(), WATCHDOG_DEBUG);
      cache_set('recurly_entity_invoice_pager', $invoices);
      break;
    }

    if ($invoice->uuid == $last_watermark) {
      // We can clear our current pager and start anew on the next run.
      cache_clear_all('recurly_entity_invoice_pager', 'cache');
      cache_clear_all('recurly_entity_invoice_first', 'cache');
      watchdog('recurly_entity', 'All Recurly adjustments have been imported.', array(), WATCHDOG_DEBUG);
      break;
    }

    // If this invoice can't be mapped to a local Recurly user, neither can the
    // associated adjustments.
    $account_code = $invoice->account->idFromHref();
    if (!RecurlyEntity::entityOwner($account_code)) {
      watchdog('recurly_entity', 'Invoice @uuid was skipped during adjustment import as it is not owned by a local Drupal user or entity.', array('@uuid' => $invoice->uuid));
      $invoices->next();
      continue;
    }

    // We've found an invoice to process.
    foreach ($invoice->line_items as $adjustment) {
      $queue = DrupalQueue::get('recurly_entity_adjustment_import');
      $queue->createItem($adjustment);
      watchdog('recurly_entity', 'Adjustment @uuid has been queued for importing.', array('@uuid' => $adjustment->uuid), WATCHDOG_DEBUG);
    }

    $invoices->next();
  }

  // In the case where there aren't any new invoices, we don't update our
  // watermark to save clearing the variables cache.
  if ($imported < $limit && $first_invoice != $last_watermark) {
    watchdog('recurly_entity', 'Setting the invoice watermark to @uuid.', array('@uuid' => $first_invoice), WATCHDOG_DEBUG);
    variable_set('recurly_entity_invoice_watermark', $first_invoice);
    cache_clear_all('recurly_entity_invoice_pager', 'cache');
    cache_clear_all('recurly_entity_invoice_first', 'cache');
  }
}

/**
 * Imports and saves all Adjustments in Recurly to our local entity tables.
 */
function recurly_entity_adjustment_import(Recurly_Adjustment $adjustment) {
  $adjustment_entity = entity_get_controller('recurly_adjustment')
    ->loadFromResource($adjustment);

  try {
    watchdog('recurly_entity', 'Saving adjustment @uuid.', array('@uuid' => $adjustment->uuid));
    $adjustment_entity->save();
    watchdog('recurly_entity', 'Saved adjustment entity ID @id for @uuid.', array('@id' => $adjustment_entity->id, '@uuid' => $adjustment_entity->uuid));
  }
  catch (RecurlyMissingOwnerException $e) {
    watchdog('recurly_entity', 'Adjustment @uuid was not imported as it is not owned by a local Drupal account or entity.', array('@uuid' => $adjustment->uuid));
  }
}
