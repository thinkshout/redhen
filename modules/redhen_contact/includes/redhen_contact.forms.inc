<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function redhen_contact_contact_form($form, &$form_state, $contact) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_contact') . '/includes/redhen_contact.forms.inc';

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
  $form['redhen_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#description' => t('Contact state.'),
    '#options' => array(
      REDHEN_STATE_ACTIVE => t('Active'),
      REDHEN_STATE_ARCHIVED => t('Archived')
    ),
    '#default_value' => $contact->redhen_state,
    '#weight' => 10,
  );

  if ($contact->user) {
    $user_uri = entity_uri('user', $contact->user);
    $form['user'] = array(
      '#type' => 'fieldset',
      '#description' => t('The Drupal user associated with this contact.'),
      array(
        'name' => array(
          '#type' => 'item',
          '#markup' => l($contact->user->name, $user_uri['path'])
        )
      ),
      array(
        'user_actions' => array(
          '#type' => 'actions',
          array(
            'unlink' => array(
              '#type' => 'button',
              '#value' => t('Unlink'),
              '#ajax' => array(
                'callback' => 'redhen_contact_unlink_user_callback',
                'wrapper' => 'edit-user'
              )
            ),
            'delete' => array(
              '#type' => 'button',
              '#value' => t('Delete'),
              '#ajax' => array(
                'callback' => 'redhen_contact_unlink_user_callback',
                'wrapper' => 'edit-user'
              )
            )
          )
        )
      )
    );
  }

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
 * Ajax callback for clearing a user from a contact.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function redhen_contact_unlink_user_callback($form, &$form_state) {
  $contact = &$form_state['redhen_contact'];
  $delete = ($form_state['clicked_button']['#parents'][0] == 'delete');
  redhen_contact_remove_contact_user($contact, $delete);
  $form['user'] = array(
    '#type' => 'fieldset',
    array(
      'message' => array(
        '#type' => 'item',
        '#markup' => t('The Drupal user has been removed from this contact')
      )
    )
  );
  return $form['user'];
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
  $contact->redhen_state = $form_state['values']['redhen_state'];

  // @TODO Add a log field to existing entries
  $contact->log = '';

  // Notify field widgets.
  field_attach_submit('redhen_contact', $contact, $form, $form_state);

  // Save the contact.
  $contact = redhen_contact_save($contact);

  drupal_set_message(t('Contact %name saved.', array('%name' => $contact->name)));
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
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_contact') . '/includes/redhen_contact.forms.inc';

  $form['#submit'][] = 'redhen_contact_contact_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $contact->name)),
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

  if (redhen_contact_delete_multiple(array($contact->contact_id))) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $contact->name)));
    watchdog('redhen_contact', 'Deleted contact %title.', array('%title' => $contact->name), WATCHDOG_NOTICE);
    $form_state['redirect'] = 'redhen/contact';
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $contact->name)), 'error');
  }
}
