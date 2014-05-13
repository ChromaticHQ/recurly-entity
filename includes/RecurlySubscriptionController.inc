<?php

/**
 * @class
 * Contains all methods that have been overridden for CRUD operations for
 * Recurly Subscriptions.
 */
class RecurlySubscriptionController extends RecurlyAPIController implements RecurlyRemoteEntity {

  /**
   * Override save() to check for a valid local subscription owner.
   */
  public function save($subscription, DatabaseTransaction $transaction = NULL) {
    if (!isset($subscription->entity_type) || !isset($subscription->entity_id)) {
      throw new RecurlySubscriptionMissingOwnerException('Subscriptions must have an associated owner entity to be saved.');
    }

    parent::save($subscription, $transaction);
  }

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
   * Map a Recurly Subscription UUID to it's entity ID.
   *
   * @param string $uuid
   *   The UUID of the subscription.
   *
   * @return int
   *   The entity ID of the subscription, or FALSE if one is not found.
   */
  public function entityId($uuid) {
    $result = $this->entityIdMultiple(array($uuid));
    return reset($result);
  }

  /**
   * Map multiple Recurly subscription UUIDs to their entity ID.
   *
   * @param array $uuids
   *   An array of Recurly subscription UUIDs.
   *
   * @return array
   *   An array of entity IDs, keyed by their subscription UUID.
   */
  public function entityIdMultiple(array $uuids) {
    $ids = entity_get_id_by_uuid('recurly_subscription', $uuids);
    // TODO: If a UUID isn't found, pull it down.
    return $ids;
  }

  /**
   * Map an Entity ID to a Recurly UUID.
   *
   * @param int $entity_id
   *   The entity ID of the subscription.
   *
   * @return string
   *   The unique Recurly UUID, or FALSE if one is not found.
   */
  public function recurlyCode($entity_id) {
    $result = $this->recurlyCodeMultiple(array($entity_id));
    return reset($result);
  }

  /**
   * Map multiple entity IDs to their Recurly UUID.
   *
   * @param array $entity_ids
   *   The array of entity IDs to map.
   *
   * @return array
   *   An array of Recurly UUIDs.
   */
  public function recurlyCodeMultiple(array $entity_ids) {
    return entity_get_uuid_by_id('recurly_subscription', $entity_ids);
  }
}

// Generic exception for when a subscription has no valid owner.
class RecurlySubscriptionMissingOwnerException extends Exception {}
