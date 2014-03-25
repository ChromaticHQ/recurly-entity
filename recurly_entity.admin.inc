<?php

/**
 * @file
 * Recurly Subscription Plan editing UI.
 */

/**
 * Form callback for the Subscription plan edit form.
 *
 * @param $subscription_plan
 *   The recurly_subscription_plan entity object to edit.
 */
function recurly_subscription_plan_form($form, &$form_state, $subscription_plan) {
  $form = array();

  $form_state['recurly_subscription_plan'] = $subscription_plan;
  $form_state['entity_type'] = 'recurly_subscription_plan';
  field_attach_form('recurly_subscription_plan', $subscription_plan, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save subscription plan'),
    '#submit' => array('recurly_subscription_plan_form_submit'),
  );

  return $form;
}

/**
 * Form submit callback for the Subscription plan edit form.
 */
function recurly_subscription_plan_form_submit(&$form, &$form_state) {
  $recurly_subscription_plan = entity_ui_form_submit_build_entity($form, $form_state);
  $recurly_subscription_plan->save();

  $form_state['redirect'] = 'admin/content/recurly_subscription_plans';
}
