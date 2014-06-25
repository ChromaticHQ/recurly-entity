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
      'accounting_code', 'invoice',
    );

    $values = array();
    foreach ($attributes as $attr) {
      if (isset($adjustment->$attr)) {
        $values[$attr] = $plan->$attr;
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
   * Map a Recurly code to an entity ID.
   *
   * @param string $code
   *   The unique Recurly code, such as an account code or plan code.
   *
   * @return int
   *   The entity ID of the recurly object, if it exists.
   */
  public function entityId($plan_code) {
    // TODO: Implement entityId() method.
  }

  /**
   * Map multiple Recurly codes to their entity ID.
   *
   * @param array $codes
   *   An array of Recurly codes.
   *
   * @return array
   *   An array of entity IDs, keyed by their code.
   */
  public function entityIdMultiple(array $plan_codes) {
    // TODO: Implement entityIdMultiple() method.
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
    // TODO: Implement recurlyCode() method.
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
    // TODO: Implement recurlyCodeMultiple() method.
  }
}
