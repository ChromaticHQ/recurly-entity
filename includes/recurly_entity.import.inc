<?php

function recurly_entity_invoice_queue($limit = 50) {
  recurly_client_initialize();

  // We cache a copy of our pager to persist between requests. Otherwise, there
  // is no way to retrieve the state of the pager without rewriting it to store
  // it's state in public methods and variables.
  if ($cached = cache_get('recurly_entity_invoice_pager')) {
    $invoices = $cached->data;
  }
  else {
    $invoices = Recurly_InvoiceList::get();
  }

  // TODO: Use cache for this variable?
  $last_watermark = variable_get('recurly_entity_invoice_watermark', "");

  $imported = 0;
  foreach ($invoices as $invoice) {
    // Keep track of what the most recent UUID is from Recurly.
    if (!isset($first_invoice)) {
      $first_invoice = $invoice->uuid;
    }

    if (++$imported > $limit) {
      // Preserve our position for the next queue run.
      cache_set('recurly_entity_invoice_pager', $invoices);
      break;
    }

    if ($invoice->uuid == $last_watermark) {
      // We can clear our current pager and start anew on the next run.
      cache_clear_all('recurly_entity_invoice_pager');
      watchdog('recurly_entity', 'All Recurly adjustments have been imported.');
      break;
    }

    // If this invoice can't be mapped to a local Recurly user, neither can the
    // associated adjustments.
    $account_code = $invoice->account->idFromHref();
    if (!RecurlyEntity::entityOwner($account_code)) {
      watchdog('recurly_entity', 'Invoice @uuid was skipped during adjustment import as it is not owned by a local Drupal user or entity.', array('@uuid' => $invoice->uuid));
      continue;
    }

    foreach ($invoice->line_items as $adjustment) {
      $queue = DrupalQueue::get('recurly_entity_adjustment_import');
      $queue->createItem($adjustment);
    }
  }

  // In the case where there aren't any new invoices, we don't update our
  // watermark to save clearing the variables cache.
  if ($first_invoice != $last_watermark) {
    variable_set('recurly_entity_invoice_watermark', $first_invoice);
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
  }
  catch (RecurlyMissingOwnerException $e) {
    watchdog('recurly_entity', 'Adjustment @uuid was not imported as it is not owned by a local Drupal account or entity.', array('@uuid' => $adjustment->uuid));
  }
}
