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
    $values = $adjustment->values();
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
}
