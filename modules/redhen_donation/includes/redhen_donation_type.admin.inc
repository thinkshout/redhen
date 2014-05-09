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

  $form['settings']['contact_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['settings']['contact_settings']['contact_type'] = array(
    '#type' => 'select',
    '#title' => t('Contact type'),
    '#default_value' => $donation_type->settings['contact_settings']['contact_type'],
    '#options' => redhen_contact_type_options_list(),
    '#empty_value' => FALSE,
    '#description' => t('Select the contact type that will be displayed inline on the donation form.'),
  );

  $form['settings']['contact_settings']['contact_update'] = array(
    '#type' => 'checkbox',
    '#options' => array(1,1),
    '#title' => t('Update and link existing contacts'),
    '#description' => t('If a contact is found with the same email as the donation order, it will be updated and linked to the new account.'),
    '#default_value' => $donation_type->settings['contact_settings']['contact_update'],
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[contact_type]"]' => array('value' => 0),
      ),
    ),
  );
  $form['settings']['contact_settings']['contact_update_fields'] = array(
    '#type' => 'checkbox',
    '#options' => array(1,1),
    '#title' => t('Update contact fields'),
    '#description' => t('Should the contact fields be updated?'),
    '#default_value' => $donation_type->settings['contact_settings']['contact_update_fields'],
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[contact_type]"]' => array('value' => 0),
      ),
      'visible' => array(
        ':input[name="settings[contact_update]"]' => array('checked' => TRUE),
      ),
    ),
  );


  $form['settings']['commerce_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Commerce Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $product_types = commerce_product_type_options_list();
  unset($product_types['recurring']);
  $form['settings']['commerce_settings']['one_time_product_type'] = array(
    '#title' => t('One time product type'),
    '#type' => 'select',
    '#options' => $product_types,
    '#default_value' => $donation_type->settings['commerce_settings']['one_time_product_type'],
  );

  $order_type_options = commerce_order_type_options_list();
  if (count($order_type_options) > 1) {
    $form['settings']['commerce_settings']['order_type'] = array(
      '#type' => 'select',
      '#title' => t('Order type.'),
      '#options' => $order_type_options,
      '#default_value' => $donation_type->settings['commerce_settings']['order_type'],
      '#description' => t('Choose an order type.'),
    );
  }

  $line_item_type_options = commerce_line_item_type_options_list();
  if (count($line_item_type_options) > 1) {
    $form['settings']['commerce_settings']['line_item_type'] = array(
      '#type' => 'select',
      '#title' => t('Line item type.'),
      '#options' => $line_item_type_options,
      '#default_value' => $donation_type->settings['commerce_settings']['line_item_type'],
      '#description' => t('Choose a line item type.'),
    );
  }

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
  $donation_type->settings = array();
  // Serialize settings
  $donation_type->settings['contact_settings'] = $donation_type->contact_settings;
  $donation_type->settings['commerce_settings'] = $donation_type->commerce_settings;
  $donation_type->save();
  $form_state['redirect'] = 'admin/structure/redhen/donation_types';
}

/**
 * Form API submit callback for the delete button.
 */
function redhen_donation_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/redhen/donation_types/manage/' . $form_state['redhen_donation_type']->name . '/delete';
}
