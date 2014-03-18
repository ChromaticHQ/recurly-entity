<?php

/**
 * @class
 * The top-level object containing common methods to all Recurly objects.
 */
class RecurlyEntity extends Entity {

  /**
   * Construct a Recurly Entity.
   *
   * The primary purpose of this is to ensure that the Recurly PHP library has
   * been loaded as it doesn't use Drupal's autoloader.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    recurly_client_initialize();
    parent::__construct($values, $entityType);
  }
}

