<?php

/**
 * RedHen Relation Role form.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 * @param RedHenRelationRole $redhen_relation_role
 *   RedHen Relation role object.
 * @param string $op
 *   Operation.
 *
 * @return mixed
 *   Form array.
 */
function redhen_relation_role_form($form, &$form_state, $redhen_relation_role, $op = 'edit') {
  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $redhen_relation_role->label,
    '#description' => t('The human-readable name of this registration state.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $redhen_relation_role->identifier(),
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'redhen_relation_role_name_exists',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for
    this redhen relation role. It must only contain lowercase letters,
     numbers, and underscores.'),
  );

  if (isset($redhen_relation_role->default_role)
    && $redhen_relation_role->default_role == 1) {
    $form['default_role_placeholder'] = array(
      '#type' => 'markup',
      '#markup' => t('Default role.'),
    );
    $form['default_role'] = array(
      '#type' => 'hidden',
      '#value' => 1,
    );
  }
  else {
    $form['default_role'] = array(
      '#title' => t('Default'),
      '#type' => 'checkbox',
      '#default_value' => isset($redhen_relation_role->default_role) ?
      $redhen_relation_role->default_role : 0,
      '#description' => t('If selected, this role will be the default for new RedHen affiliations.'),
    );
  }

  $options = array();
  $permissions = redhen_relation_role_permissions();
  foreach ($permissions as $name => $permission) {
    $options[$name] = $permission['label'];
  }
  $form['permissions'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Permissions'),
    '#options' => $options,
    '#default_value' => isset($redhen_relation_role->permissions) ?
    $redhen_relation_role->permissions : array(),
    '#description' => t('Optional special permissions to grant this role.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save relation role'),
    '#weight' => 40,
  );

  return $form;
}

/**
 * Form API validation callback for the relation role form.
 */
function redhen_relation_role_form_validate(&$form, &$form_state) {
  // Check for at least one default.
  if ($form_state['values']['default_role'] == 0) {
    $id = isset($form_state['redhen_relation_role']->redhen_relation_role_id) ?
      $form_state['redhen_relation_role']->redhen_relation_role_id :
      0;
    $default_roles_query = db_select('redhen_relation_role')
      ->condition('default_role', 1)
      ->condition('redhen_relation_role_id', $id, '<>');
    $default_roles_count = $default_roles_query
      ->countQuery()->execute()->fetchField();
    if ($default_roles_count == 0) {
      form_set_error('default_role', t('Must have one default role.'));
    }
  }

}

/**
 * Form API submit callback for the relation role form.
 */
function redhen_relation_role_form_submit(&$form, &$form_state) {
  $redhen_relation_role = entity_ui_form_submit_build_entity($form, $form_state);
  $redhen_relation_role->save();
  $form_state['redirect'] = 'admin/structure/redhen/relation_roles';
}