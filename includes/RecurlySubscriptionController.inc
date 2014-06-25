<?php

/**
 * @class
 * Contains all methods that have been overridden for CRUD operations for
 * Recurly Subscriptions.
 */
class RecurlySubscriptionController extends RecurlyAPIController implements RecurlyRemoteEntity {

  /**
   * @{inheritdoc}
   *
   * @return RecurlySubscription
   */
  public function loadFromRecurly($uuid) {
    recurly_client_initialize();
    $subscription = Recurly_Subscription::get($uuid);

    $values = $subscription->getValues();

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

  /**
   * Load a Recurly Subscription by it's UUID.
   *
   * @param string $uuid
   *   The UUID of the subscription.
   *
   * @return RecurlySubscription
   *   The loaded Recurly subscription, or FALSE if one is not found.
   */
  public function loadByUuid($uuid) {
    $entities = $this->loadByUuidCodeMultiple(array($uuid));
    return reset($entities);
  }

  /**
   * Load a Recurly Subscriptions by UUIDs.
   *
   * @param array $uuids
   *   An array of Subscription UUIDs.
   *
   * @return array
   *   An array of loaded recurly subscriptions.
   */
  public function loadByUUIDMultiple(array $uuid) {
    $entity_ids = $this->entityIdMultiple($uuid);
    return $this->load($entity_ids);
  }


  /**
   * Create a new Recurly subscription and return the saved subscription
   * entity.
   *
   * To use this, create a Recurly_Subscription object like normal, but instead
   * of calling create() on it directly pass it through to this method.
   *
   * @param Recurly_Subscription $recurly_subscription
   *   The Recurly_Subscription to create.
   *
   * @throws RecurlySubscriptionNotActiveException
   *
   * @return RecurlySubscription
   *   The RecurlySubscription entity that was created.
   */
  public function createRecurlySubscription(Recurly_Subscription $recurly_subscription) {
    module_invoke_all('recurly_subscription_precreate', $recurly_subscription);
    $recurly_subscription->create();
    module_invoke_all('recurly_subscription_create', $recurly_subscription);

    if ($recurly_subscription->state != 'active') {
      throw new RecurlySubscriptionNotActiveException('The subscription was not activated and has not been successfully charged.');
    }

    // TODO: For simplicity we reload the subscription from Recurly, but this
    // could be optimized by modifying the RecurlySubscriptionController to either
    // save to the entity cache, or to refactor loadFromRecurly() to allow for an
    // existing Recurly_Subscription object to be "loaded" from.
    $subscription_entity = $this->loadFromRecurly($recurly_subscription->uuid);
    $this->save($subscription_entity);

    return $subscription_entity;
  }

  /**
   * Load all subscriptions by the entity that owns the subscription.
   *
   * @param string $entity_type
   *   The type of entity, typically 'user'.
   * @param int $entity_id
   *   The id of the owner.
   *
   * @return array
   *   An array of subscriptions associated with this entity, or an empty array
   *   if none were found.
   */
  public function loadByOwner($entity_type, $entity_id) {
    $query = new EntityFieldQuery();
    $results = $query->entityCondition('entity_type', 'recurly_subscription')
      ->propertyCondition('entity_type', $entity_type)
      ->propertyCondition('entity_id', $entity_id)
      ->execute();

    if (!empty($results['recurly_subscription'])) {
      return $this->load(array_keys($results['recurly_subscription']));
    }

    return array();
  }
}

class RecurlySubscriptionNotActiveException extends Exception {}
