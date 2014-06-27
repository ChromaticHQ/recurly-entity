<?php

/**
 * @class
 * Expose metadata from Adjustments to the entity API.
 */
class RecurlyAdjustmentMetadataController extends EntityDefaultMetadataController {

  /**
   * Overrides EntityDefaultMetadataController::entityPropertyInfo().
   */
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    return $info;
  }
}
