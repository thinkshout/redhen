<?php
// $Id$

/**
 * @file
 */

/**
 * Form callback: create or edit a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object to edit or for a create form an empty bundle type
 *     object with properties instantiated but not populated.
 */
function entity_manager_bundle_type_form($form, &$form_state, $bundle_type) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'entity_manager') . '/includes/entity_manager.forms.inc';

  // Store the initial bundle type in the form state.
  $form_state['bundle_type'] = $bundle_type;
  $entity_info = entity_get_info($bundle_type->entity_type);

  $form['bundle_type'] = array(
    '#tree' => TRUE,
  );

  $form['bundle_type']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $bundle_type->name,
    '#description' => t('The human-readable name of this @label type. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.', array('@label' => $entity_info['label'])),
    '#required' => TRUE,
    '#size' => 32,
    '#field_suffix' => ' <small id="edit-bundle-type-name-suffix">' . t('Machine name: @type', array('@type' => $bundle_type->type)) . '</small>',
  );

  if (empty($bundle_type->type)) {
    $form['bundle_type']['type'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#default_value' => $bundle_type->type,
      '#maxlength' => 32,
      '#required' => TRUE,
      '#machine_name' => array(
        'exists' => 'entity_manager_machine_name_verify', // Use the dummy callback and validate ourselves
        'source' => array('bundle_type', 'name'),
      ),
      '#description' => t('The machine-readable name of this @label type. This name must contain only lowercase letters, numbers, and underscores, it must be unique.', array('@label' => $entity_info['label'])),
    );
  }

  $form['bundle_type']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Describe this @label type. The text will be displayed on the <em>Add new content</em> page.', array('@label' => $entity_info['label'])),
    '#default_value' => $bundle_type->description,
    '#rows' => 3,
  );

  $form['bundle_type']['help'] = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#description' => t('This text will be displayed at the top of the page when creating or editing @label of this type.', array('@label' => $entity_info['label'])),
    '#default_value' => $bundle_type->help,
    '#rows' => 3,
  );

  $form['bundle_type']['title_pattern'] = array(
    '#type' => 'textfield',
    '#title' => t('Title pattern'),
    '#default_value' => $bundle_type->title_pattern,
    '#description' => t('The title pattern to use for @label type. Allowed tokens are displayed below.', array('@label' => $entity_info['label'])),
    '#required' => TRUE,
    '#element_validate' => array('token_element_validate'),
    '#token_types' => array('redhen-contact')
  );

  // Add the token tree UI.
  $form['token_tree'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('redhen-contact'),
    '#show_restricted' => TRUE,
    '#weight' => 5,
  );

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
    '#value' => t('Save @label type', array('@label' => $entity_info['label'])),
    '#submit' => $submit + array('entity_manager_bundle_type_form_submit'),
  );

  if (!empty($form_state['bundle_type']->type)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete @label type', array('@label' => $entity_info['label'])),
      '#suffix' => l('Cancel', $entity_info['managed options']['bundles']['path']),
      '#submit' => $submit + array('entity_manager_bundle_type_form_delete_submit'),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['save_continue'] = array(
      '#type' => 'submit',
      '#value' => t('Save and add fields'),
      '#suffix' => l('Cancel', $entity_info['managed options']['bundles']['path']),
      '#submit' => $submit + array('entity_manager_bundle_type_form_submit'),
      '#weight' => 45,
    );
  }

  $form['#validate'][] = 'entity_manager_bundle_type_form_validate';

  return $form;
}

/**
 * Validation callback for entity_manager_bundle_type_form().
 *
 * @see entity_manager_machine_name_verify
 */
function entity_manager_bundle_type_form_validate($form, &$form_state) {
  $bundle_type = $form_state['bundle_type'];

  // If saving a new bundle type, ensure it has a unique machine name.
  // We have to do this ourselves since we need both bundle type and the
  // entity type, and the machine_name form element doesn't allow additional
  // arguents to be passed.
  if (empty($bundle_type->type)) {
    if (!entity_manager_validate_bundle_type_unique($form_state['values']['bundle_type']['type'], $bundle_type->entity_type)) {
      form_set_error('bundle_type][type', 'The machine name specified is already in use.');
    }
  }
}

/**
 * Form submit handler: save a bundle type.
 */
function entity_manager_bundle_type_form_submit($form, &$form_state) {
  $bundle_type = $form_state['bundle_type'];
  $entity_info = entity_get_info($bundle_type->entity_type);
  $updated = !empty($bundle_type->type);

  foreach ($form_state['values']['bundle_type'] as $key => $value) {
    $bundle_type->$key = $value;
  }

  // Write the bundle type to the database.
  $bundle_type->is_new = !$updated;
  entity_manager_bundle_type_save($bundle_type);

  // Redirect based on the button clicked.
  drupal_set_message(t('@label type saved.', array('@label' => $entity_info['label'])));

  if ($form_state['clicked_button']['#parents'][0] == 'save_continue') {
    $form_state['redirect'] = $entity_info['managed options']['bundles']['path'] . '/' . strtr($form_state['bundle_type']->type, array('_' => '-')) . '/fields';
  }
  else {
    $form_state['redirect'] = $entity_info['managed options']['bundles']['path'];
  }
}

/**
 * Submit callback for delete button on entity_manager_bundle_type_form().
 *
 * @see entity_manager_bundle_type_form()
 */
function entity_manager_bundle_type_form_delete_submit($form, &$form_state) {
  $bundle_type = $form_state['bundle_type'];
  $entity_info = entity_get_info($bundle_type->entity_type);
  $form_state['redirect'] = $entity_info['managed options']['bundles']['path'] . '/' . strtr($form_state['bundle_type']->type, array('_' => '-')) . '/delete';
}

/**
 * Form callback: confirmation form for deleting a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object to be deleted.
 *
 * @see confirm_form()
 */
function entity_manager_bundle_type_delete_form($form, &$form_state, $bundle_type) {
  $form_state['bundle_type'] = $bundle_type;
  $entity_info = entity_get_info($bundle_type->entity_type);

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'entity_manager') . '/includes/entity_manager.forms.inc';

  $form['#submit'][] = 'entity_manager_bundle_type_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete the %name @label type?', array('%name' => $bundle_type->name, '@label' => $entity_info['label'])),
    $entity_info['managed options']['bundles']['path'],
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for entity_manager_bundle_type_delete_form().
 */
function entity_manager_bundle_type_delete_form_submit($form, &$form_state) {
  $bundle_type = $form_state['bundle_type'];
  $entity_info = entity_get_info($bundle_type->entity_type);

  entity_manager_bundle_type_delete($bundle_type->type, $bundle_type->entity_type);

  drupal_set_message(t('The @label type %name has been deleted.', array('%name' => $bundle_type->name, '@label' => $entity_info['label'])));
  watchdog('entity_manager', 'Deleted @label type %name.', array('%name' => $bundle_type->name, '@label' => $entity_info['label']), WATCHDOG_NOTICE);

  $form_state['redirect'] = $entity_info['managed options']['bundles']['path'];
}

/**
 * Form callback: confirmation form for deleting a bundle type.
 *
 * @param $bundle_type
 *   The bundle type object to be deleted.
 *
 * @see confirm_form()
 */
function entity_manager_bundle_type_reset_form($form, &$form_state, $bundle_type) {
  $form_state['bundle_type'] = $bundle_type;
  $entity_info = entity_get_info($bundle_type->entity_type);

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'entity_manager') . '/includes/entity_manager.forms.inc';

  $form['#submit'][] = 'entity_manager_bundle_type_reset_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to reset the %name @label type?', array('%name' => $bundle_type->name, '@label' => $entity_info['label'])),
    $entity_info['managed options']['bundles']['path'],
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for entity_manager_bundle_type_delete_form().
 */
function entity_manager_bundle_type_reset_form_submit($form, &$form_state) {
  $bundle_type = $form_state['bundle_type'];
  $entity_info = entity_get_info($bundle_type->entity_type);

  entity_manager_bundle_type_delete($bundle_type->type, $bundle_type->entity_type);

  drupal_set_message(t('The @label type %name has been reset.', array('%name' => $bundle_type->name, '@label' => $entity_info['label'])));
  watchdog('entity_manager', 'Reset @label type %name.', array('%name' => $bundle_type->name, '@label' => $entity_info['label']), WATCHDOG_NOTICE);

  $form_state['redirect'] = $entity_info['managed options']['bundles']['path'];
}
