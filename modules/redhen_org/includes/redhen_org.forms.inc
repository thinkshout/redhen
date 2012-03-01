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
  $form['name'] = array();
  $form['name']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $org->label,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
  );
  $form['redhen_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#description' => t('Organization state.'),
    '#options' => array(1 => t('Active'), 0 => t('Archived')), //@TODO: these should use the defined constants
    '#default_value' => $org->redhen_state,
    '#weight' => 10,
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

  // Set the contact's author uid
  global $user;
  $org->author_uid = $user->uid;

  // Save default parameters back into the $org object.
  $org->label = $form_state['values']['label'];
  $org->redhen_state = $form_state['values']['redhen_state'];

  // @TODO Add a log field to existing entries
  $org->log = '';

  // Notify field widgets.
  field_attach_submit('redhen_org', $org, $form, $form_state);

  // Save the organization.
  $org->save();

  drupal_set_message(t('Organization saved.'));
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

  $title = $org->label;

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $title)),
    '',
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

  $title = $org->label;

  // No status returned from delete(), assume it worked
  $org->delete();
  drupal_set_message(t('%title has been deleted.', array('%title' => $title)));
  watchdog('redhen_org', 'Deleted organization %title.', array('%title' => $title), WATCHDOG_NOTICE);

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
    t('Are you sure you want to change the primary contact for %org to %name?', array('%org' => $org->label, '%name' => $contact->name)),
    '',
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
    drupal_set_message(t('%contact_name has been set as the primary contact for %org_name.', array('%contact_name' => $contact_name, '%org_name' => $org_name)));
  }
  else {
    drupal_set_message(t('Unable to set %contact_name as the primary contact for %org_name.', array('%contact_name' => $contact_name, '%org_name' => $org_name)));
  }

  drupal_goto(drupal_get_destination());

}

/**
 * Returns the form to filter org lists.
 */
function redhen_org_filter_form($form, $form_state, $entity_info, $property_info) {
  $form = array();

  if (!empty($property_info['properties'])) {
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
    foreach($property_info['properties'] as $key => $prop) {
      $form['filter_fieldset']['filters']['properties'][$key] = array(
        '#type' => $prop['type'],
        '#title' => $prop['label'],
        '#default_value' => isset($_GET[$key]) ? $_GET[$key] : NULL
      );

      if ($prop['type'] == 'select' && !empty($prop['options'])) {
        $form['filter_fieldset']['filters']['properties'][$key]['#options'] = $prop['options'];
      }
    }

    // add org type; will load additional field filters on change
    $type_options = array('' => t('-- Any --'));
    foreach($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }
    $form['filter_fieldset']['filters']['type'] = array(
      '#type' => 'select',
      '#title' => t('Organization type'),
      '#options' => $type_options,
      '#default_value' => isset($_GET['type']) ? $_GET['type'] : NULL,
      '#ajax' => array(
        'callback' => 'redhen_org_get_fields_callback',
        'wrapper' => 'redhen_org_fields_container'
      )
    );

    $form['filter_fieldset']['filters']['fields'] = array(
      '#prefix' => '<div id="redhen_org_fields_container">',
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
      '#submit' => array('redhen_org_filter_form_reset')
    );
  }

  // add the fields on an ajax type change event or if a filter is being passed
  if (!empty($form_state['values']['filters']['type']) || isset($_GET['type'])) {
    $type = !empty($form_state['values']['filters']['type']) ?
      $form_state['values']['filters']['type'] : $_GET['type'];

    $fields = field_info_instances('redhen_org', $type);
    if (!empty($fields)) {
      $form['filter_fieldset']['filters']['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Additional Fields for @type', array('@type' => $type)),
        '#collapsible' => FALSE,
        '#id' => 'redhen_org_fields_container'
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
function redhen_org_get_fields_callback($form, $form_state) {
  return $form['filter_fieldset']['filters']['fields'];
}

/**
 * Submit handler for redhen_org_filter_form().
 */
function redhen_org_filter_form_submit($form, $form_state) {
  drupal_goto($_GET['q'], array('query' => $form_state['values']['filters']));
}

function redhen_org_filter_form_reset() {
  drupal_goto('redhen/org');
}
