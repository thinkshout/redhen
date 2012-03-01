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
    '#options' => array(1 => t('Active'), 0 => t('Archived')), //@TODO: these should use the defined constants
    '#default_value' => $contact->redhen_state,
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
  $contact->redhen_state = $form_state['values']['redhen_state'];

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
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_contact') . '/includes/redhen_contact.forms.inc';

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
 * Returns the form to filter contact lists.
 */
function redhen_contact_filter_form($form, $form_state, $entity_type) {
  $form = array();

  $property_info = entity_get_all_property_info($entity_type);

  if (!empty($property_info)) {
    $form['filter_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Filters'),
      '#collapsible' => TRUE,
      // expand fieldset if the list was filtered
      '#collapsed' => (empty($_GET['properties']) && empty($_GET['fields']))
    );

    $form['filter_fieldset']['filters'] = array(
      '#type' => 'container',
      '#tree' => TRUE
    );

    $form['filter_fieldset']['filters']['properties'] = array(
      '#type' => 'container',
    );

    // add a form field for each property
    foreach($property_info as $key => $prop) {
      if (isset($prop['filter']) && $prop['filter']) {
        $type = isset($prop['field_type']) ? $prop['field_type'] : $prop['type'];
        $form['filter_fieldset']['filters']['properties'][$key] = array(
          '#type' => $type,
          '#title' => $prop['label'],
          '#default_value' => isset($_GET['properties'][$key]) ? $_GET['properties'][$key] : NULL
        );
        if ($type == 'select' && isset($prop['options'])) {
          $form['filter_fieldset']['filters']['properties'][$key]['#options'] = $prop['options'];
        }
      }
    }

    // add contact type; will load additional field filters on change
    $type_options = array('' => t('-- Any --'));
    $entity_info = entity_get_info($entity_type);
    foreach($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }
    $form['filter_fieldset']['filters']['bundle'] = array(
      '#type' => 'select',
      '#title' => t('Contact type'),
      '#options' => $type_options,
      '#default_value' => isset($_GET['bundle']) ? $_GET['bundle'] : NULL,
      '#ajax' => array(
        'callback' => 'redhen_contact_fields_callback',
        'wrapper' => 'redhen_contact_fields_container',
        'method' => 'replace',
        'effect' => 'fade',
        'progress' => array(
          'type' => 'throbber',
          'message' => t('Retrieving fields for this connection type.')
        )
      )
    );

    $form['filter_fieldset']['filters']['fields'] = array(
      '#prefix' => '<div id="redhen_contact_fields_container">',
      '#suffix' => '</div>'
    );

    $form['filter_fieldset']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Apply')
    );

    $form['filter_fieldset']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_contact_filter_form_reset')
    );
  }

  // add the fields on an ajax type change event or if a filter is being passed
  if (!empty($form_state['values']['filters']['bundle']) || isset($_GET['bundle'])) {
    $bundle = !empty($form_state['values']['filters']['bundle']) ?
      $form_state['values']['filters']['bundle'] : $_GET['bundle'];

    $fields = field_info_instances('redhen_contact', $bundle);
    if (!empty($fields)) {
      $form['filter_fieldset']['filters']['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Additional Fields for @bundle', array('@bundle' => $bundle)),
        '#collapsible' => FALSE,
        '#id' => 'redhen_contact_fields_container'
      );

      foreach($fields as $key => $field) {
        $form['filter_fieldset']['filters']['fields'][$key] = array(
          '#type' => 'textfield',
          '#title' => $field['label'],
          '#default_value' => isset($_GET['fields'][$key]) ? $_GET['fields'][$key] : NULL,
        );
      }
    }
  }

  return $form;
}

/**
 * AJAX callback to return fields for a given type.
 */
function redhen_contact_fields_callback($form, $form_state) {
  return $form['filter_fieldset']['filters']['fields'];
}

/**
 * Submit handler for redhen_contact_filter_form().
 */
function redhen_contact_filter_form_submit($form, $form_state) {
  $filters = $form_state['values']['filters'];
  drupal_goto($_GET['q'], array('query' => $filters));
}

function redhen_contact_filter_form_reset() {
  drupal_goto($_GET['q']);
}
