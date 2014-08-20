<?php

class RecurlyAdjustment extends RecurlyEntity {

  /**
   * Override __construct to force an entity type.
   */
  public function __construct(array $values = array(), $entityType = 'recurly_adjustment') {
    // Adjustments are not modifiable by the user.
    $this->status = ENTITY_FIXED;

    // TODO: Can this be moved up to RecurlyEntity for everything with a UUID?
    // We need to map to a local ID if possible. This can happen if a plan has
    // been saved locally but our entity cache is empty.
    if (!isset($values['id'])) {
      $id = entity_get_controller('recurly_adjustment')
        ->entityId($values['uuid']);
      if ($id) {
        $values['id'] = $id;
      }
    }

    // Generate a title for this adjustment.
    if (!isset($values['title'])) {
      $values['title'] = substr($values['description'], 0, 255);
    }

    $this->setOwner($values);

    // Pop out the stub invoice into a UUID.
    if (isset($values['invoice'])) {
      recurly_client_initialize();
      $invoice = $values['invoice']->get();
      $values['invoice'] = $invoice->uuid;
      $values['invoice_number'] = $invoice->invoice_number;
    }

    // Convert the created at date into a DATETIME.
    $values['created_at'] = $values['created_at']
      ->setTimezone(new DateTimeZone('UTC'))
      ->format('U');

    parent::__construct($values, $entityType);
  }

  /**
   * Overrides uri().
   */
  public function uri() {
    return array(
      'path' => 'recurly/adjustment/' . $this->id,
    );
  }

  /**
   * Validate that this adjustment has been collected.
   *
   * This method will update field_adjustment_collected if Recurly has
   * collected the adjustment since it was imported. If the adjustment is a
   * credit, then it will always be considered as "collected", but the
   * collected field will only be updated when the adjustment is actually
   * applied to an invoice.
   *
   * @throws RecurlyEntityInvoiceRequiredException
   */
  public function collected() {
    /** @var EntityDrupalWrapper $adjustment_w */
    $adjustment_w = entity_metadata_wrapper('recurly_adjustment', $this);
    // This adjustment is already marked as collected, so skip checking with
    // Recurly.
    if ($this->field_adjustment_collected == TRUE) {
      return;
    }

    // Only validate invoiced charges. We allow pending credits to pass this
    // method.
    if ($this->state == 'invoiced') {
      $invoice = Recurly_Invoice::get($this->invoice_number);
      $this->invoiceCollected($invoice);
      $adjustment_w->field_adjustment_collected->set(TRUE);
      $adjustment_w->save();
    }
    elseif ($this->unit_amount_in_cents > 0) {
      throw new RecurlyEntityInvoiceRequiredException(format_string('Adjustment !uuid has not been invoiced yet.', array('!uuid' => $this->uuid)));
    }
  }

}
