<?php

class RecurlySubscription extends RecurlyEntity {

  /**
   * Override __construct to force an entity type.
   */
  public function __construct(array $values = array(), $entityType = 'recurly_subscription') {
    // TODO: Can this be moved up to RecurlyEntity for everything with a UUID?
    // We need to map to a local ID if possible. This can happen if a plan has
    // been saved locally but our entity cache is empty.
    if (!isset($values['id'])) {
      $id = entity_get_controller('recurly_subscription')
        ->entityId($values['uuid']);
      if ($id) {
        $values['id'] = $id;
      }
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

    parent::__construct($values, $entityType);
  }

  /**
   * Override uri() for use with entity_uri().
   */
  public function uri() {
    return array(
      'path' => 'recurly/subscription/' . $this->id,
    );
  }
}

