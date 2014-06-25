<?php

class RecurlyAdjustmentController extends RecurlyAPIController {

  /**
   * Load an entity from Recurly using it's unique identifier.
   *
   * @param string $uuid
   *   The UUID of the adjustment to load.
   *
   * @return RecurlyAdjustment
   *   The adjustment that was loaded.
   */
  public function loadFromRecurly($uuid) {
    recurly_client_initialize();
    $adjustment = Recurly_Adjustment::get($uuid);
    $attributes = array(
      'currency','unit_amount_in_cents','quantity','description',
      'accounting_code','invoice','account',
    );

    $values = array();
    foreach ($attributes as $attr) {
      if (isset($adjustment->$attr)) {
        $values[$attr] = $adjustment->$attr;
      }
    }

    $adjustment_entity = new RecurlyAdjustment($values);

    // Update our local entity cache, but only if we have saved this adjustment
    // locally.
    if (isset($adjustment_entity->id)) {
      // Attach our fields.
      $wrapper = array($adjustment_entity->id => $adjustment_entity);
      $this->attachLoad($wrapper);
      $this->entityCacheSet(array($adjustment_entity));
    }

    return $adjustment_entity;
  }

  /**
   * Map a uuid to an entity ID.
   *
   * @param string $uuid
   *   The UUID of the adjustment.
   *
   * @return int
   *   The entity ID of the recurly object, if it exists.
   */
  public function entityId($uuid) {
    $result = $this->entityIdMultiple(array($uuid));
    return reset($result);
  }

  /**
   * Map multiple Recurly UUIDs to their entity ID.
   *
   * @param array $uuids
   *   An array of adjustment UUIDs.
   *
   * @return array
   *   An array of entity IDs, keyed by their UUID.
   */
  public function entityIdMultiple(array $uuids) {
    $ids = entity_get_id_by_uuid('recurly_subscription', $uuids);
    // TODO: If a UUID isn't found, pull it down.
    return $ids;
  }

  /**
   * Map an Entity ID to a Recurly code.
   *
   * @param int $entity_id
   *   The entity ID of the recurly object.
   *
   * @return string
   *   The unique Recurly code.
   */
  public function recurlyCode($entity_id) {
    $result = $this->recurlyCodeMultiple(array($entity_id));
    return reset($result);
  }

  /**
   * Map an array of Entity IDs to Recurly codes.
   *
   * @param array $entity_ids
   *   The array of entity IDs to map.
   *
   * @return array
   *   An array of recurly codes.
   */
  public function recurlyCodeMultiple(array $entity_ids) {
    return entity_get_uuid_by_id('recurly_adjustment', $entity_ids);
  }
}
