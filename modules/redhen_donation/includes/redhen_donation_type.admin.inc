<?php

/**
 * @file
 * Donation type editing UI.
 */

/**
 * Generates the donation type editing form.
 */
function redhen_donation_type_form($form, &$form_state, $donation_type, $op = 'edit') {
  if ($op == 'clone') {
    $donation_type->label .= ' (cloned)';
    $donation_type->name = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $donation_type->label,
    '#description' => t('The human-readable name of this donation type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($donation_type->name) ? $donation_type->name : '',
    '#maxlength' => 32,
    '#disabled' => $donation_type->locked && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'redhen_donation_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this donation type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save donation type'),
    '#weight' => 40,
  );

  if (!$donation_type->locked && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete donation type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_donation_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function redhen_donation_type_form_submit(&$form, &$form_state) {
  $donation_type = entity_ui_form_submit_build_entity($form, $form_state);
  $donation_type->save();
  $form_state['redirect'] = 'admin/structure/redhen/donation_types';
}

/**
 * Form API submit callback for the delete button.
 */
function redhen_donation_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/redhen/donation_types/manage/' . $form_state['redhen_donation_type']->name . '/delete';
}
