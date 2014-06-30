<?php

function recurly_entity_adjustment_import() {
  recurly_client_initialize();
  $invoices = Recurly_InvoiceList::get();
  $last_watermark = variable_get('recurly_entity_invoice_watermark', "");
  $adjustments = 0;
  foreach ($invoices as $invoice) {
    // Keep track of what the most recent UUID is from Recurly.
    if (!isset($first_invoice)) {
      $first_invoice = $invoice->uuid;
    }

    // If we have seen this invoice, we can raise our mark up to the first
    // invoice we saw.
    if ($invoice->uuid == $last_watermark) {
      break;
    }

    foreach ($invoice->line_items as $adjustment) {
      $adjustments++;
      watchdog('recurly_entity', 'Imported @count adjustments.', array('@count' => $adjustments));
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
  }

  // In the case where there aren't any new invoices, we don't update our
  // watermark to save clearing the variables cache.
  if ($first_invoice != $last_watermark) {
    variable_set('recurly_entity_invoice_watermark', $first_invoice);
  }
}
