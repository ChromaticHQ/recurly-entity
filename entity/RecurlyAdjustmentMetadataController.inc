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
    $properties = &$info['recurly_adjustment']['properties'];
    $properties['created_at']['type'] = 'date';
    return $info;
  }
}
