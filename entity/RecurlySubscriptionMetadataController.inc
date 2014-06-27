<?php

/**
 * @class RecurlySubscriptionMetadataController
 * This describes our entity metadata using EntityAPI OOP-style notation instead
 * of function hooks. Less code in .modules, less code required per given page
 * load.
 */
class RecurlySubscriptionMetadataController extends EntityDefaultMetadataController {

  /**
   * Overrides EntityDefaultMetadataController::entityPropertyInfo().
   */
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    return $info;
  }
}
