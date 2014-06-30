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

  /**
   * Map the Recurly account from the subscription to a local owner ID.
   *
   * Most of the time this will be a user id, but the Recurly module allows
   * accounts to be mapped to any entity type.
   *
   * @param string $account_code
   *   The Recurly account code to find the entity IDs for.
   *
   * @return array
   *   An array with the entity type and entity ID.
   */
  public static function entityOwner($account_code) {
    $local_account = recurly_account_load(array('account_code' => $account_code), TRUE);
    if ($local_account) {
      return array(
        'entity_type' => $local_account->entity_type,
        'entity_id' => $local_account->entity_id,
      );
    }
  }

  /**
   * Set the owner of this entity.
   *
   * This is commonly used when constructing Recurly entities.
   *
   * @param array &$values
   *   The array of values used to construct the entity, from Entity API. If an
   *   owner entity is found, 'entity_type' and 'entity_id' keys will be added.
   */
  protected function setOwner(array &$values) {
    if (!isset($values['entity_type']) || !isset($values['entity_id'])) {
      // See if this object has an account_code; that might save us from having
      // to do a remote account load.
      if (isset($values['account_code'])) {
        $account_code = $values['account_code'];
      }
      else {
        $account_code = $values['account']->idFromHref();
      }

      if ($owner = $this->entityOwner($account_code)) {
        $values = array_merge($values, $owner);
      }
    }
  }
}

