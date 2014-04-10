<?php

/**
 * @class
 * Contains all common methods for RecurlyAPI controllers. For example, this
 * contains the logic which handles EntityCache integration common to all
 * Recurly entities.
 */
abstract class RecurlyAPIController extends EntityAPIController implements RecurlyRemoteEntity {

  /**
   * Override the load() method to fetch our objects from Recurly first and then
   * store them in the entity cache. We can then call the parent method to
   * query our base table for any cached entities.
   *
   * Note that if conditions() are specified, that the query will only match
   * entities that have been previously cached.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();
    $original_ids = $ids;

    if (!empty($this->entityInfo['entity cache']) && $ids && !$conditions) {
      $cached_entities = EntityCacheControllerHelper::entityCacheGet($this, $ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      $ids = array_diff($ids, array_keys($cached_entities));
      $entities += $cached_entities;

      // Add loaded entities to the static cache if we are not loading a
      // revision.
      if ($this->cache && !empty($cached_entities)) {
        $this->cacheSet($cached_entities);
      }
    }
    if (!($this->cacheComplete && $ids === FALSE && !$conditions) && ($ids === FALSE || $ids)) {
      // Note that we attach fields in loadFromRecurly, right before we store
      // the plan in the entity cache.
      $entities = $this->loadFromRecurlyMultiple($this->recurlyCodeMultiple($ids));
    }

    // This isn't as bad for performance as it seems as we statically cache
    // fetched entities from above.
    return parent::load($original_ids, $conditions);
  }

  /**
   * Store entities in the entity cache.
   *
   * @param array $entities
   *   An array of RecurlySubscriptionPlan entities.
   */
  public function entityCacheSet(array $entities) {
    if (!empty($this->entityInfo['entity cache']) && !empty($entities)) {
      EntityCacheControllerHelper::entityCacheSet($this, $entities);
    }
  }
}
