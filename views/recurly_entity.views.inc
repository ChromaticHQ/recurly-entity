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

    $data['recurly_adjustment'][$entity_type] = _recurly_entity_relationship_data('Recurly adjustment', $entity_type);
  }

  return $data;
}

