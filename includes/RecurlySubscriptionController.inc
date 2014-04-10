<?php

/**
 * @class
 * Contains all methods that have been overridden for CRUD operations for
 * Recurly Subscriptions.
 */
class RecurlySubscriptionController extends RecurlyAPIController implements RecurlyRemoteEntity {

  /**
   * Helper to load multiple subscriptions at once.
   *
   * @param array $uuids
   *   An array of Recurly Subscription uuids.
   *
   * @return array
   *   An array of RecurlySubscription entities.
   */
  public function loadFromRecurlyMultiple(array $uuids) {
    $entities = array();
    foreach ($uuids as $uuid) {
      $entities[] = $this->loadFromRecurly($uuid);
    }

    return $entities;
  }

  /**
   * @{inheritdoc}
   *
   * @return RecurlySubscriptionPlan
   */
  public function loadFromRecurly($uuid) {
    recurly_client_initialize();
    $subscription = Recurly_Subscription::get($uuid);

    // Recurly_Subscription::$_writeableAttributes is protected :(
    $attributes = array(
      'account','plan_code','coupon_code','unit_amount_in_cents','quantity',
      'currency','starts_at','trial_ends_at','total_billing_cycles', 'first_renewal_date',
      'timeframe', 'subscription_add_ons', 'net_terms', 'po_number', 'collection_method'
    );
    // TODO: Recurly_Subscription::$_nestedAttributes = array('account', 'subscription_add_ons');

    $values = array();
    foreach ($attributes as $attr) {
      // We can probably remove this later, but for now it greatly simplifies
      // testing.
      if (isset($subscription->$attr) && is_scalar($subscription->$attr)) {
        $values[$attr] = $subscription->$attr;
      }
    }

    $subscription_entity = new RecurlySubscription($values);

    // Update our local entity cache, but only if we have saved this plan
    // locally.
    if (isset($subscription_entity->id)) {
      // Attach our fields.
      $wrapper = array($subscription_entity->id => $subscription_entity);
      $this->attachLoad($wrapper);
      $this->entityCacheSet(array($subscription_entity));
    }

    return $subscription_entity;
  }

  // TODO: Plan codes, replace with uuid wrappers, nothing?
  /**
   * Load a Recurly Subscription Plan by it's plan code.
   *
   * @param string $plan_code
   *   The unique Recurly plan code.
   *
   * @return RecurlySubscriptionPlan
   *   The loaded recurly subscription plan, or FALSE if one is not found.
   */
  public function loadByPlanCode($plan_code) {
    $entities = $this->loadByPlanCodeMultiple(array($plan_code));
    return reset($entities);
  }

  /**
   * Load a Recurly Subscription Plans by plan codes.
   *
   * @param array $plan_codees
   *   An array of unique Recurly plan codes.
   *
   * @return array
   *   An array of loaded recurly subscription plans.
   */
  public function loadByPlanCodeMultiple(array $plan_codes) {
    $entity_ids = $this->entityIdMultiple($plan_codes);
    return $this->load($entity_ids);
  }
  // END TODO

  /**
   * Map a Recurly Subscription Plan code to it's entity ID.
   *
   * @param string $plan_code
   *   The unique Recurly plan code.
   *
   * @return int
   *   The entity ID of the subscription plan, or FALSE if one is not found.
   */
  public function entityId($plan_code) {
    // TODO
    return FALSE;
    $result = $this->entityIdMultiple(array($plan_code));
    return reset($result);
  }

  /**
   * Map multiple Recurly plan codes to their entity ID.
   *
   * @param array $plan_codes
   *   An array of Recurly subscription plan codes.
   *
   * @return array
   *   An array of entity IDs, keyed by their plan code.
   */
  public function entityIdMultiple(array $plan_codes) {
    // We don't bother with the static cache here as we only key that by entity
    // ID, and not by plan code. If needed we can add a second static cache by
    // plan code later.
    // TODO
    return array();

    return db_select('recurly_subscription_plan', 's')
      ->fields('s', array('plan_code', 'id'))
      ->condition('s.plan_code', $plan_codes, 'IN')
      ->execute()
      ->fetchAllKeyed();
  }

  /**
   * Map an Entity ID to a Recurly plan code.
   *
   * @param int $entity_id
   *   The entity ID of the subscription plan.
   *
   * @return string
   *   The unique Recurly plan code, or FALSE if one is not found.
   */
  public function recurlyCode($entity_id) {
    // TODO
    return FALSE;
    $result = $this->recurlyCodeMultiple(array($entity_id));
    return reset($result);
  }

  /**
   * Map multiple entity IDs to their Recurly plan codes.
   *
   * @param array $entity_ids
   *   The array of entity IDs to map.
   *
   * @return array
   *   An array of recurly plan codes.
   */
  public function recurlyCodeMultiple(array $entity_ids) {
    // TODO
    return array();
    $plan_codes = array();

    // First, check our static entity cache for any plans that have been loaded.
    $plans = $this->cacheGet($entity_ids);

    // We don't use array_map() and a closure because array_map() copies the
    // array as of PHP 5.3. php--
    // http://willem.stuursma.name/2010/11/22/a-detailed-look-into-array_map-and-foreach/
    foreach ($plans as $id => $plan) {
      $plan_codes[$id] = $plan->plan_code;
    }

    // Now, for any IDs that aren't in our static, cache, make a database call.
    $missed_ids = array_diff($entity_ids, array_keys($plans));

    if (!empty($missed_ids)) {
      $plan_codes += db_select('recurly_subscription_plan', 's')
        ->fields('s', array('id', 'plan_code'))
        ->condition('s.id', $missed_ids, 'IN')
        ->execute()
        ->fetchAllKeyed();
    }

    return $plan_codes;
  }
}
