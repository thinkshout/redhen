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
 * Form API submit callback for the relation role form.
 */
function redhen_relation_role_form_submit(&$form, &$form_state) {
  $redhen_relation_role = entity_ui_form_submit_build_entity($form, $form_state);
  $redhen_relation_role->save();
  $form_state['redirect'] = 'admin/structure/redhen/relation_roles';
}
