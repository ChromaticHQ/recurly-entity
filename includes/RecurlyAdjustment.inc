<?php

class RecurlyAdjustment extends RecurlyEntity {

  /**
   * Override __construct to force an entity type.
   */
  public function __construct(array $values = array(), $entityType = 'recurly_adjustment') {
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

    // Map the Recurly account from the subscription to a local owner ID. Most
    // of the time this will be a user id, but the Recurly module allows
    // accounts to be mapped to any entity type.
    if (!isset($values['entity_type']) || !isset($values['entity_id'])) {
      $account = $values['account']->get();
      $local_account = recurly_account_load(array('account_code' => $account->account_code), TRUE);
      if ($local_account) {
        $values['entity_type'] = $local_account->entity_type;
        $values['entity_id'] = $local_account->entity_id;
      }
    }

    // Pop out the stub invoice into a UUID.
    $values['invoice'] = $values['invoice']->get()->uuid;

    parent::__construct($values, $entityType);
  }
}
