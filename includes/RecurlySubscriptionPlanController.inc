<?php

/**
 * @class
 * Contains all methods that have been overridden for CRUD operations for
 * Recurly Subscription Plans.
 */
class RecurlySubscriptionPlanController extends EntityAPIController implements RecurlyRemoteEntity {

  /**
   * Override the load() method to fetch our objects from Recurly first and then
   * store them in the entity cache. We can then call the parent method to
   * query our base table for any cached entities.
   *
   * Note that if conditions() are specified, that the query will only match
   * entities that have been previously cached.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();
    $original_ids = $ids;

    if (!empty($this->entityInfo['entity cache']) && $ids && !$conditions) {
      $cached_entities = EntityCacheControllerHelper::entityCacheGet($this, $ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      $ids = array_diff($ids, array_keys($cached_entities));
      $entities += $cached_entities;

      // Add loaded entities to the static cache if we are not loading a
      // revision.
      if ($this->cache && !empty($cached_entities)) {
        $this->cacheSet($cached_entities);
      }
    }
    if (!($this->cacheComplete && $ids === FALSE && !$conditions) && ($ids === FALSE || $ids)) {
      // Note that we attach fields in loadFromRecurly, right before we store
      // the plan in the entity cache.
      $entities = $this->loadFromRecurlyMultiple($this->recurlyCodeMultiple($ids));
    }

    // This isn't as bad for performance as it seems as we statically cache
    // fetched entities from above.
    return parent::load($original_ids, $conditions);
  }

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
   * Helper to load multiple subscription plans at once.
   *
   * @param array $plan_codes
   *   An array of Recurly Subscription plan codes.
   *
   * @return array
   *   An array of RecurlySubscriptionPlan entities.
   */
  public function loadFromRecurlyMultiple(array $plan_codes) {
    $entities = array();
    foreach ($plan_codes as $plan_code) {
      $entities[] = $this->loadFromRecurly($plan_code);
    }

    return $entities;
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

    // Update our local entity cache, but only if we have saved this plan
    // locally.
    if (isset($plan_entity->id)) {
      // Attach our fields.
      $wrapper = array($plan_entity->id => $plan_entity);
      $this->attachLoad($wrapper);
      $this->entityCacheSet(array($plan_entity));
    }

    return $plan_entity;
  }

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
