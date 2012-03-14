<?php

/**
 * @file
 * Forms for creating, editing, and deleting memberships.
 */

/**
 * Form callback: create or edit a contact.
 *
 * @param $membership
 *   The membership object to edit or for a create form an empty membership object
 *     with only a membership type defined.
 * @param RedhenContact|RedhenOrg $entity
 * @param string $entity_type
 */
function redhen_membership_membership_form($form, &$form_state, RedhenMembership $membership = NULL, $entity = NULL, $entity_type = '') {

  // add jquery libs and
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'redhen') . '/js/redhen.forms.js');

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_membership') . '/includes/redhen_membership.forms.inc';

  $membership_types = redhen_membership_get_types();

  // instantiate new membership based on default or submitted membership type
  if (!isset($membership)) {
    $membership_type = isset($form_state['values']['type']) ?
      $form_state['values']['type'] :
      reset($membership_types)->name;
    $values = array();
    $values['type'] = $membership_type;
    $membership = entity_create('redhen_membership', $values);
  }

  // Store membership and entity data for use on submit
  $form_state['membership'] = $membership;
  $form_state['entity'] = $entity;

  // Add the default field elements.
  $form['note'] = array(
    '#type' => 'textfield',
    '#title' => t('Note'),
    '#default_value' => $membership->note,
    '#maxlength' => 255,
    '#required' => FALSE,
    '#weight' => -10,
  );

  $form['start_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Start Date'),
    '#description' => t('If set, the membership will be considered valid after this date.'),
    '#default_value' => isset($membership->start_date) ? date('m/d/Y', strtotime($membership->start_date)) : '',
    '#attributes' => array('class' => array("redhen-field-type-date")),
    '#weight' => -9,
  );

  $form['expire_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration Date'),
    '#description' => t('If set, the membership will be considered valid before this date.'),
    '#default_value' => isset($membership->expire_date) ? date('m/d/Y', strtotime($membership->expire_date)) : '',
    '#attributes' => array('class' => array("redhen-field-type-date")),
    '#weight' => -8,
  );

  $form['redhen_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#description' => t('Membership state. Archived memberships do not confer benefits to contacts/users. Start and expiration dates will not change this setting. '),
    '#options' => array(
      REDHEN_STATE_ACTIVE => t('Active'),
      REDHEN_STATE_ARCHIVED => t('Archived')
    ),
    '#default_value' => $membership->redhen_state,
    '#weight' => -7,
  );

  if (!empty($form_state['values']['type'])) {
    $membership->type = $form_state['values']['type'];
  }
  $options = array();
  foreach($membership_types as $type) {
    $options[$type->name] = $type->label;
  }
  $form['type'] = array(
    '#title' => t('Membership type'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($membership) ? $membership->type : NULL,
    '#weight' => -6,
    '#ajax' => array(
      'callback' => 'redhen_membership_membership_type_callback',
      'wrapper' => 'redhen_membership_fields',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'throbber',
        'message' => t('Retrieving fields for this membership type.')
      )
    ),
  );

  $form['fields'] = array(
    '#type' => 'fieldset',
    '#id' => 'redhen_membership_fields'
  );

  // Add the field related form elements.
  field_attach_form('redhen_membership', $membership, $form['fields'], $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 40,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save membership'),
    '#submit' => $submit + array('redhen_membership_membership_form_submit'),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'redhen_membership_membership_form_validate';

  return $form;
}

/**
 * AJAX callback to return fields for a given relation type.
 */
function redhen_membership_membership_type_callback($form, $form_state) {
  return $form['fields'];
}

/**
 * Validation callback for redhen_membership_membership_form().
 */
function redhen_membership_membership_form_validate($form, &$form_state) {
  $membership = $form_state['membership'];

  $start = strtotime($form_state['values']['start_date']);
  $expire = strtotime($form_state['values']['expire_date']);

  // check that expire date is after start date if both are set
  if ( ($start != '') && ($expire != '') && ($start >= $expire) ) {
    form_set_error('expire_date', 'Expiration date must be after the start date.');
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_membership', $membership, $form, $form_state);
}

/**
 * Submit callback for redhen_membership_membership_form().
 */
function redhen_membership_membership_form_submit($form, &$form_state) {
  global $user;

  $membership = $form_state['membership'];
  if (isset($form_state['entity'])) {
    $entity = $form_state['entity'];
  } else {
    $entity = $membership->entity;
  }
  $entity_type = $entity->entityType();
  $entity_uri = entity_uri($entity_type, $entity);

  // Set the membership's author uid
  $membership->author_uid = $user->uid;

  // Save default parameters back into the $membership object.
  $membership->type = $form_state['values']['type'];
  $membership->note = $form_state['values']['note'];
  $membership->start_date = ($form_state['values']['start_date'] != '') ? date('Y-m-d', strtotime($form_state['values']['start_date'])) : NULL;
  $membership->expire_date = ($form_state['values']['expire_date'] != '') ? date('Y-m-d', strtotime($form_state['values']['expire_date'])) : NULL;
  $membership->redhen_state = $form_state['values']['redhen_state'];
  $membership->entity_type = $entity_type;
  $membership->entity_id = entity_id($entity_type, $entity);

  // Notify field widgets.
  field_attach_submit('redhen_membership', $membership, $form, $form_state);

  // Save the membership.
  $membership->save();

  drupal_set_message(t('Membership %label saved.', array('%label' => $membership->label())));
  $form_state['redirect'] = $entity_uri['path'] . '/memberships';
}

/**
 * Form callback: confirmation form for deleting a membership.
 *
 * @param $membership
 *   The membership object to be deleted.
 *
 * @see confirm_form()
 */
function redhen_membership_membership_delete_form($form, &$form_state, $membership) {

  $form_state['membership'] = $membership;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_membership') . '/includes/redhen_membership.forms.inc';

  $form['#submit'][] = 'redhen_membership_membership_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete %label?', array('%label' => $membership->label())),
    '',
    '<p>' . t('Deleting this membership cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for redhen_membership_membership_delete_form().
 */
function redhen_membership_membership_delete_form_submit($form, &$form_state) {
  $membership = $form_state['membership'];

   // No return status from delete() unfortunately
  $membership->delete();
  drupal_set_message(t('%label has been deleted.', array('%label' => $membership->label())));
  watchdog('redhen_membership', 'Deleted membership %label.', array('%label' => $membership->label()), WATCHDOG_NOTICE);

  drupal_goto();

}
