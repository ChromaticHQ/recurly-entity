<?php

/**
 * @class
 * Contains all methods that have been overridden for CRUD operations for
 * Recurly Subscription Plans.
 */
class RecurlySubscriptionPlanController extends EntityAPIController implements RecurlyRemoteEntity {

  /**
   * Store entities in the entity cache.
   *
   * @param array $entities
   *   An array of RecurlySubscriptionPlan entities.
   */
  public function entityCacheSet(array $entities) {
    if (!empty($this->entityInfo['entity cache']) && !empty($entities)) {
      EntityCacheControllerHelper::entityCacheSet($this, $entities);
    }
  }

  /**
   * @{inheritdoc}
   *
   * @return RecurlySubscriptionPlan
   */
  public function loadFromRecurly($plan_code) {
    recurly_client_initialize();
    $plan = Recurly_Plan::get($plan_code);

    // Recurly_Plan::$_writeableAttributes is protected :(
    $attributes = array(
      'plan_code','name','description','success_url','cancel_url',
      'display_donation_amounts','display_quantity','display_phone_number',
      'bypass_hosted_confirmation','unit_name','payment_page_tos_link',
      'plan_interval_length','plan_interval_unit','trial_interval_length',
      'trial_interval_unit','unit_amount_in_cents','setup_fee_in_cents',
      'total_billing_cycles','accounting_code'
    );

    $values = array();
    foreach ($attributes as $attr) {
      // We can probably remove this later, but for now it greatly simplifies
      // testing.
      if (isset($plan->$attr) && is_scalar($plan->$attr)) {
        $values[$attr] = $plan->$attr;
      }
    }

    $plan_entity = new RecurlySubscriptionPlan($values);
    $this->entityCacheSet(array($plan_entity));
    return $plan;
  }

  /**
   * TODO: Make this a single load function that wraps around a multiple load
   * function.
   *
   * TODO: Integrate this with loadFromRecurly().
   */
  public function loadByPlanCode($plan_code) {
    // TODO: Check our static cache first.
    $id = db_select('recurly_subscription_plan', 's')
      ->fields('s', array('id'))
      ->condition('s.plan_code', $plan_code)
      ->execute()
      ->fetchField();

    if ($id) {
      $entities = $this->load(array($id));
      return reset($entities);
    }
  }
}

