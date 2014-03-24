<?php

class RecurlySubscriptionPlanMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $info[$this->type]['properties']['name'] = array(
      'label' => t('Name'),
      'type' => 'string',
      'description' => t('Subscription name'),
      'setter callback' => 'entity_property_verbatim_set',
    );
    $info[$this->type]['properties']['plan_code'] = array(
      'label' => t('Plan code'),
      'type' => 'string',
      'description' => t('Subscription plan code'),
      'schema field' => 'plan_code',
      'setter callback' => 'entity_property_verbatim_set',
    );
    return $info;
  }
}

