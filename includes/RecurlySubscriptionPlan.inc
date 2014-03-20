<?php

/**
 * @class
 * The entity representing a Subscription Plan.
 */
class RecurlySubscriptionPlan extends RecurlyEntity {

  /**
   * Override __construct to force an entity type.
   */
  public function __construct(array $values = array(), $entityType = 'recurly_subscription_plan') {
    // We need to map to a local ID if possible. This can happen if a plan has
    // been saved locally but our entity cache is empty.
    if (!$values['id']) {
      $id = entity_get_controller('recurly_subscription_plan')
        ->entityId($values['plan_code']);
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
      'path' => 'recurly/subscription-plan/' . $this->id,
    );
  }
}

