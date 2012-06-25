<?php

/**
 * @file
 * Forms for creating, editing, and deleting organizations.
 */

function redhen_org_add($type) {
  $org = entity_get_controller('redhen_org')->create(array('type' => $type));
  return drupal_get_form('redhen_org_org_form', $org);
}

/**
 * Form callback: create or edit a contact.
 *
 * @param $org
 *   The organization object to edit or for a create form an empty organization object
 *     with only an org type defined.
 */
function redhen_org_org_form($form, &$form_state, $org) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_org') . '/includes/redhen_org.forms.inc';

  // Add the default field elements.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $org->label,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
  );

  // Add the field related form elements.
  $form_state['redhen_org'] = $org;
  field_attach_form('redhen_org', $org, $form, $form_state);

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
    '#value' => t('Save organization'),
    '#submit' => $submit + array('redhen_org_org_form_submit'),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'redhen_org_org_form_validate';

  return $form;
}

/**
 * Validation callback for redhen_org_org_form().
 */
function redhen_org_org_form_validate($form, &$form_state) {
  $org = $form_state['redhen_org'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_org', $org, $form, $form_state);
}

/**
 * Submit callback for redhen_org_org_form().
 */
function redhen_org_org_form_submit($form, &$form_state) {
  $org = &$form_state['redhen_org'];

  // Set the org's author uid
  global $user;
  $org->author_uid = $user->uid;

  // Save default parameters back into the $org object.
  $org->label = $form_state['values']['label'];

  // Notify field widgets.
  field_attach_submit('redhen_org', $org, $form, $form_state);

  // Save the organization.
  $org->save();

  drupal_set_message(t('Organization %label saved.', array('%label' => $org->label)));
  $form_state['redirect'] = 'redhen/org/' . $org->org_id;
}

/**
 * Form callback: confirmation form for deleting an organization.
 *
 * @param $org
 *   The organization object to be deleted.
 *
 * @see confirm_form()
 */
function redhen_org_org_delete_form($form, &$form_state, $org) {
  $form_state['redhen_org'] = $org;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_org') . '/includes/redhen_org.forms.inc';

  $form['#submit'][] = 'redhen_org_org_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete %label?', array('%label' => $org->label)),
    entity_uri('redhen_org', $org),
    '<p>' . t('Deleting this organization cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for redhen_org_org_delete_form().
 */
function redhen_org_org_delete_form_submit($form, &$form_state) {
  $org = $form_state['redhen_org'];

  redhen_org_delete_multiple(array($org->org_id));
  drupal_set_message(t('%label has been deleted.', array('%label' => $org->label)));
  watchdog('redhen_org', 'Deleted organization %label.', array('%label' => $org->label), WATCHDOG_NOTICE);

}

/**
 * Form callback: confirmation form for archiving an organization.
 *
 * @param $org
 *   The organization object to be archived.
 *
 * @see confirm_form()
 */
function redhen_org_org_archive_form($form, &$form_state, $org) {
  $form_state['redhen_org'] = $org;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_org') . '/includes/redhen_org.forms.inc';

  $form['#submit'][] = 'redhen_org_org_archive_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to archive %label?', array('%label' => $org->label)),
    entity_uri('redhen_org', $org),
    '<p>' . t('Archiving this organization cannot be undone.') . '</p>',
    t('Archive'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for redhen_org_org_archive_form().
 */
function redhen_org_org_archive_form_submit($form, &$form_state) {
  $org = $form_state['redhen_org'];

  if ($org->setState(REDHEN_STATE_ARCHIVED)) {
    drupal_set_message(t('%label has been archived.', array('%label' => $org->label)));
  }
  else {
    drupal_set_message(t('%label was not archived.', array('%label' => $org->label)), WATCHDOG_ERROR);
  }
  $url = entity_uri($org->entityType(), $org);
  drupal_goto($url['path']);
}

/**
 * Form callback: confirmation form for changing the primary contact.
 *
 * @param $org
 *   The organization object to be deleted.
 * @param $contact
 *   The contact being made the primary contact.
 *
 * @see confirm_form()
 */
function redhen_org_set_primary_form($form, &$form_state, $org, $contact) {
  $form_state['redhen_org'] = $org;
  $form_state['redhen_contact'] = $contact;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_org') . '/includes/redhen_org.forms.inc';

  $form['#submit'][] = 'redhen_org_set_primary_form_submit';

  $existing = '';

  if (isset($org->primary_contact->name)) {
    $existing = '<p>' . t('%old_primary will no longer be the primary contact.', array('%old_primary' => $org->primary_contact->name)) . '</p>';
  }

  $form = confirm_form($form,
    t('Are you sure you want to change the primary contact for %org to %name?', array(
      '%org' => $org->label,
      '%name' => $contact->name
    )),
    entity_uri('redhen_org', $org),
    $existing,
    t('Change'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for redhen_org_set_primary_form().
 */
function redhen_org_set_primary_form_submit($form, &$form_state) {
  $org = $form_state['redhen_org'];
  $contact = $form_state['redhen_contact'];
  $org_name = $org->label;
  $contact_name = $contact->name;

  if ($org->setPrimaryContact($contact)) {
    drupal_set_message(t('%contact_name has been set as the primary contact for %org_name.', array(
      '%contact_name' => $contact_name,
      '%org_name' => $org_name
    )));
  }
  else {
    drupal_set_message(t('Unable to set %contact_name as the primary contact for %org_name.', array(
      '%contact_name' => $contact_name,
      '%org_name' => $org_name
    )));
  }

  drupal_goto(drupal_get_destination());

}
