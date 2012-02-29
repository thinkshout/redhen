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
 */
function redhen_membership_membership_form($form, &$form_state, RedhenMembership $membership = NULL, RedhenOrg $org = NULL) {
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
    $values['org_id'] = isset($org) ? $org->org_id : NULL;
    $membership = entity_create('redhen_membership', $values);
  }

  // Store membership entity for use on submit
  $form_state['membership'] = $membership;

  $options = array();
  foreach($membership_types as $type) {
    $options[$type->name] = $type->label;
  }
  $form['type'] = array(
    '#title' => t('Membership type'),
    '#weight' => -999,
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($membership) ? $membership->type : NULL,
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

  $form['org_id'] = array(
    '#title' => t('Organization'),
    '#weight' => -50,
    '#type' => 'textfield',
    '#default_value' => isset($membership->org) ?
      $membership->org->label . ' (' . $membership->org_id . ')' :
      NULL,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#autocomplete_path' => 'redhen/org/autocomplete',
    '#element_validate' => array('_redhen_membership_autocomplete_validate'),
  );

  // Add the default field elements.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $membership->label,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
  );

  $form['redhen_state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#description' => t('Membership state.'),
    '#options' => array(1 => t('Active'), 0 => t('Archived')), //@TODO: these should use the defined constants
    '#default_value' => $membership->redhen_state,
    '#weight' => 10,
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

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_membership', $membership, $form, $form_state);
}

/**
 * Submit callback for redhen_membership_membership_form().
 */
function redhen_membership_membership_form_submit($form, &$form_state) {
  global $user;

  $membership = $form_state['membership'];
  $org_id = $form_state['values']['org_id'];

  // Set the contact's author uid
  $membership->author_uid = $user->uid;

  // Save default parameters back into the $membership object.
  $membership->type = $form_state['values']['type'];
  $membership->label = $form_state['values']['label'];
  $membership->redhen_state = $form_state['values']['redhen_state'];
  $membership->org_id = $org_id;

  // Notify field widgets.
  field_attach_submit('redhen_membership', $membership, $form, $form_state);

  // Save the contact.
  redhen_membership_save($membership);

  drupal_set_message(t('Membership saved.'));
  $form_state['redirect'] = 'redhen/org/' . $org_id . '/memberships/' . $membership->membership_id;
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

  $title = $membership->label;

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $title)),
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

  $title = $membership->label;

   // No return status from delete() unfortunately
  $membership->delete();
  drupal_set_message(t('%title has been deleted.', array('%title' => $title)));
  watchdog('redhen_membership', 'Deleted membership %title.', array('%title' => $title), WATCHDOG_NOTICE);

  drupal_goto('redhen/org/' . $membership->org_id . '/memberships');

}

/**
 * Returns the form to filter membership lists.
 */
function redhen_membership_filter_form($form, $form_state, $entity_info) {
  $form = array();

  if (!empty($entity_info['properties'])) {
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
    foreach($entity_info['properties'] as $key => $prop) {
      $form['filter_fieldset']['filters']['properties'][$key] = array(
        '#type' => $prop['type'],
        '#title' => $prop['title'],
        '#default_value' => isset($_GET[$key]) ? $_GET[$key] : NULL
      );

      if ($prop['type'] == 'select' && !empty($prop['options'])) {
        $form['filter_fieldset']['filters']['properties'][$key]['#options'] = $prop['options'];
      }
    }

    // add membership type; will load additional field filters on change
    $type_options = array('' => t('-- Any --'));
    foreach($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }
    $form['filter_fieldset']['filters']['type'] = array(
      '#type' => 'select',
      '#title' => t('membership type'),
      '#options' => $type_options,
      '#default_value' => isset($_GET['type']) ? $_GET['type'] : NULL,
      '#ajax' => array(
        'callback' => 'redhen_membership_get_fields_callback',
        'wrapper' => 'redhen_membership_fields_container'
      )
    );

    $form['filter_fieldset']['filters']['fields'] = array(
      '#prefix' => '<div id="redhen_membership_fields_container">',
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
      '#submit' => array('redhen_membership_filter_form_reset')
    );
  }

  // add the fields on an ajax type change event or if a filter is being passed
  if (!empty($form_state['values']['filters']['type']) || isset($_GET['type'])) {
    $type = !empty($form_state['values']['filters']['type']) ?
      $form_state['values']['filters']['type'] : $_GET['type'];

    $fields = field_info_instances('redhen_membership', $type);
    if (!empty($fields)) {
      $form['filter_fieldset']['filters']['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Additional Fields for @type', array('@type' => $type)),
        '#collapsible' => FALSE,
        '#id' => 'redhen_membership_fields_container'
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
function redhen_membership_get_fields_callback($form, $form_state) {
  return $form['filter_fieldset']['filters']['fields'];
}

/**
 * Submit handler for redhen_membership_filter_form().
 */
function redhen_membership_filter_form_submit($form, $form_state) {
  drupal_goto($_GET['q'], array('query' => $form_state['values']['filters']));
}

function redhen_membership_filter_form_reset() {
  drupal_goto('redhen/membership');
}

function _redhen_membership_autocomplete_validate($element, &$form_state, $form) {
  // If a value was entered into the autocomplete...
  $value = '';
  if (!empty($element['#value'])) {
    // Take "label (entity id)', match the id from parenthesis.
    if (preg_match("/.+\((\d+)\)/", $element['#value'], $matches)) {
      $value = $matches[1];
    }
  }

  // Update the value of this element so the field can validate the org IDs.
  form_set_value($element, $value, $form_state);
}
