<?php

/**
 * @interface
 * Define all methods that should be implemented by any Recurly entity.
 */
interface RecurlyRemoteEntity {

  /**
   * Load an entity from Recurly using it's unique identifier.
   *
   * @param $id
   *   The ID of the entity to load, such as the plan_code for a Subscription
   *   Plan or the account_code for an Account.
   */
  public function loadFromRecurly($id);

  /**
   * Map a Recurly code to an entity ID.
   *
   * @param string $code
   *   The unique Recurly code, such as an account code or plan code.
   *
   * @return int
   *   The entity ID of the recurly object, if it exists.
   */
  public function entityId($plan_code);

  /**
   * Map an Entity ID to a Recurly code.
   *
   * @param int $entity_id
   *   The entity ID of the recurly object.
   *
   * @return string
   *   The unique Recurly code.
   */
  public function recurlyCode($entity_id);

}

