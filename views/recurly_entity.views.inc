<?php

/**
 * Implements hook_views_data_alter().
 */
function recurly_entity_views_data_alter(&$data) {
  $info = entity_get_info();

  foreach (array_keys($info) as $entity_type) {
    // Don't allow a relationship to ourselves.
    if ($entity_type == 'recurly_adjustment') {
      continue;
    }

    $entity_info = $info[$entity_type];
    $data['recurly_adjustment'][$entity_type] = _recurly_entity_relationship_data('recurly_adjustment', $entity_type);
    $data['recurly_adjustment']['table']['join'] = _recurly_entity_relationship_join($entity_info);
  }

  return $data;
}

/**
 * Return the relationship data to join to an arbitrary entity type.
 *
 * @param string $base_entity_type
 *   The base entity type being joined from, such as 'recurly_adjustment'.
 * @param string $join_entity_type
 *   The entity type being joined to, such as 'user'.
 *
 * @return array
 *   An array suitable for using with a 'join' key in a table definition.
 */
function _recurly_entity_relationship_data($base_entity_type, $join_entity_type) {
  $base_entity_info = entity_get_info($base_entity_type);
  $join_entity_info = entity_get_info($join_entity_type);
  $description = t('@join-entity from @base-entity', array('@join-entity' => $join_entity_info['label'], '@base-entity' => $base_entity_info['label']));

  // 'entity type' is a key not from Views directly, but used to determine
  // what entity type need to filter on in our relationship handler.
  return array(
    'title' => $join_entity_info['label'],
    'help' => t('The @entity-type owner of this adjustment.', array('@entity-type' => $join_entity_type)),
    'relationship' => array(
      'entity type' => $join_entity_type,
      'base' => $join_entity_info['base table'],
      'base field' => $join_entity_info['entity keys']['id'],
      'field' => 'entity_id',
      'handler' => 'recurly_entity_owner_handler',
      'label' => $description,
      'title' => $description,
    ),
  );
}

/**
 * Return an array suitable for the 'join' key on a table definition.
 *
 * @param $join_entity_info
 *   The entity info for the entity type being joined to.
 *
 * @return array
 *   An array with relationship join information.
 */
function _recurly_entity_relationship_join($join_entity_info) {
  return array(
    $join_entity_info['base table'] => array(
      'left_field' => $join_entity_info['entity keys']['id'],
      'field' => 'entity_id',
    )
  );
}

