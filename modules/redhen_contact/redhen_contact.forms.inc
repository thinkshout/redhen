<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Return a form for adding a contact.
 *
 * @param string $type 
 *   The type of contact to return a form for.
 */
function redhen_contact_add_page($type = 'default') {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the rehen_contact/add listing if only one contact type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('redhen_contact_add_list', array('content' => $content));
}

function redhen_contact_add($type) {
  $contact = entity_get_controller('redhen_contact')->create(array('type' => $type));
  return drupal_get_form('redhen_contact_contact_form', $contact);
}

/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function redhen_contact_contact_form($form, &$form_state, $contact) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_contact') . '/redhen_contact.forms.inc';

  // Add the default field elements.
  $form['name'] = array();
  $form['name']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => $contact->first_name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
  );
  $form['name']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => $contact->last_name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => 5,
  );
  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#description' => t('Contact status.'),
    '#options' => array(1 => t('Active'), 0 => t('Archived')), //@TODO: these should use the defined constants
    '#default_value' => $contact->status,
    '#weight' => 10,
  );
  

  // Add the field related form elements.
  $form_state['redhen_contact'] = $contact;
  field_attach_form('redhen_contact', $contact, $form, $form_state);

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
    '#value' => t('Save contact'),
    '#submit' => $submit + array('redhen_contact_contact_form_submit'),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'redhen_contact_contact_form_validate';

  return $form;
}

/**
 * Validation callback for redhen_contact_contact_form().
 */
function redhen_contact_contact_form_validate($form, &$form_state) {
  $contact = $form_state['redhen_contact'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_contact', $contact, $form, $form_state);
}

/**
 * Submit callback for redhen_contact_contact_form().
 */
function redhen_contact_contact_form_submit($form, &$form_state) {
  $contact = &$form_state['redhen_contact'];

  // Set the contact's author uid
  global $user;
  $contact->author_uid = $user->uid;

  // Save default parameters back into the $contact object.
  $contact->first_name = $form_state['values']['first_name'];
  $contact->last_name = $form_state['values']['last_name'];
  $contact->status = $form_state['values']['status'];

  // @TODO Add a log field to existing entries
  $contact->log = '';

  // Notify field widgets.
  field_attach_submit('redhen_contact', $contact, $form, $form_state);

  // Save the contact.
  $contact = redhen_contact_save($contact);

  drupal_set_message(t('Contact saved.'));
  $form_state['redirect'] = 'redhen/contact/' . $contact->contact_id;
}

/**
 * Form callback: confirmation form for deleting a contact.
 *
 * @param $contact
 *   The contact object to be deleted.
 *
 * @see confirm_form()
 */
function redhen_contact_contact_delete_form($form, &$form_state, $contact) {
  $form_state['redhen_contact'] = $contact;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_contact') . '/redhen_contact.forms.inc';

  $form['#submit'][] = 'redhen_contact_contact_delete_form_submit';

  $title = $contact->last_name . ', '. $contact->first_name;

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $title)),
    '',
    '<p>' . t('Deleting this contact cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for redhen_contact_contact_delete_form().
 */
function redhen_contact_contact_delete_form_submit($form, &$form_state) {
  $contact = $form_state['redhen_contact'];

  $title = $contact->last_name . ', '. $contact->first_name;

  if (redhen_contact_delete_multiple(array($contact->contact_id))) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $title)));
    watchdog('redhen_contact', 'Deleted contact %title.', array('%title' => $title), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $title)), 'error');
  }
}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_redhen_contact_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any contact types yet. Go to the <a href="@create-content">content type creation page</a> to add a new content type.', array('@create-content' => url('admin/structure/redhen/contact-types'))) . '</p>';
  }
  return $output;
}
