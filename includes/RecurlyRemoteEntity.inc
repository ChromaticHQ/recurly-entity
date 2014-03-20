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
}

