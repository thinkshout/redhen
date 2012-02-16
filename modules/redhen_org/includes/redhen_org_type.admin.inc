<?php

/**
 * @file
 * Redhen Organization type editing UI.
 */

/**
 * Generates the Redhen Organization type editing form.
 */
function redhen_org_type_form($form, &$form_state, $redhen_org_type, $op = 'edit') {
  if ($op == 'clone') {
    $redhen_org_type->label .= ' (cloned)';
    $redhen_org_type->name = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $redhen_org_type->label,
    '#description' => t('The human-readable name of this model type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($redhen_org_type->name) ? $redhen_org_type->name : '',
    '#maxlength' => 32,
    '#disabled' => $redhen_org_type->locked && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'redhen_org_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this redhen_org type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save redhen_org type'),
    '#weight' => 40,
  );

  if (!$redhen_org_type->locked && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete redhen_org type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_org_type_form_submit_delete')
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function redhen_org_type_form_submit(&$form, &$form_state) {
  $redhen_org_type = entity_ui_form_submit_build_entity($form, $form_state);
  $redhen_org_type->save();
  $form_state['redirect'] = 'admin/structure/redhen_org_types';
}

/**
 * Form API submit callback for the delete button.
 */
function redhen_org_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/redhen_org_types/manage/' . $form_state['redhen_org_type']->type . '/delete';
}