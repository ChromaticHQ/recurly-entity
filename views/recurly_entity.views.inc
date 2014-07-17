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
    // 'entity type' is a key not from Views directly, but used to determine
    // what entity type need to filter on in our relationship handler.
    $data['recurly_adjustment'][$entity_type] = array(
      'title' => $entity_info['label'],
      'help' => t('The @entity-type owner of this adjustment.', array('@entity-type' => $entity_type)),
      'relationship' => array(
        'entity type' => $entity_type,
        'base' => $entity_info['base table'],
        'base field' => $entity_info['entity keys']['id'],
        'field' => 'entity_id',
        'handler' => 'recurly_entity_owner_handler',
        'label' => t('@entity-type from adjustment', array('@entity-type' => $entity_info['label'])),
        'title' => t('@entity-type from adjustment', array('@entity-type' => $entity_info['label'])),
      ),
    );
    $data['recurly_adjustment']['table']['join'][$entity_info['base table']] = array(
      'left_field' => $entity_info['entity keys']['id'],
      'field' => 'entity_id',
    );
  }

  return $data;
}
