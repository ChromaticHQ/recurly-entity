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

