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

  /**
   * Override defaultLabel().
   */
  public function defaultLabel() {
    return $this->plan_code;
  }
}

