<?php

/**
 * @file
 * Forms for creating, editing, and deleting memberships.
 */

/**
 * Return a form for adding a membership.
 *
 * @param string $type
 *   The type of membership to return a form for.
 */
function redhen_membership_add_page($type = 'default') {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the redhen_membership/add listing if only one membership type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('redhen_membership_add_list', array('content' => $content));
}

function redhen_membership_add($type) {
  $membership = entity_get_controller('redhen_membership')->create(array('type' => $type));
  return drupal_get_form('redhen_membership_membership_form', $membership);
}

/**
 * Form callback: create or edit a contact.
 *
 * @param $membership
 *   The membership object to edit or for a create form an empty membership object
 *     with only a membership type defined.
 */
function redhen_membership_membership_form($form, &$form_state, $membership) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'redhen_membership') . '/includes/redhen_membership.forms.inc';

  // Add the default field elements.
  $form['name'] = array();
  $form['name']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $membership->label,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
  );
  $form['disposition'] = array(
    '#type' => 'select',
    '#title' => t('Disposition'),
    '#description' => t('Membership disposition.'),
    '#options' => array(1 => t('Active'), 0 => t('Archived')), //@TODO: these should use the defined constants
    '#default_value' => $membership->disposition,
    '#weight' => 10,
  );


  // Add the field related form elements.
  $form_state['redhen_membership'] = $membership;
  field_attach_form('redhen_membership', $membership, $form, $form_state);

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
 * Validation callback for redhen_membership_membership_form().
 */
function redhen_membership_membership_form_validate($form, &$form_state) {
  $membership = $form_state['redhen_membership'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_membership', $membership, $form, $form_state);
}

/**
 * Submit callback for redhen_membership_membership_form().
 */
function redhen_membership_membership_form_submit($form, &$form_state) {
  $membership = &$form_state['redhen_membership'];

  // Set the contact's author uid
  global $user;
  $membership->author_uid = $user->uid;

  // Save default parameters back into the $membership object.
  $membership->label = $form_state['values']['label'];
  $membership->disposition = $form_state['values']['disposition'];

  // @TODO Add a log field to existing entries
  $membership->log = '';

  // Notify field widgets.
  field_attach_submit('redhen_membership', $membership, $form, $form_state);

  // Save the contact.
  $membership = redhen_membership_save($membership);

  drupal_set_message(t('Membership saved.'));
  $form_state['redirect'] = 'redhen/membership/' . $membership->membership_id;
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
  $form_state['redhen_membership'] = $membership;

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
  $membership = $form_state['redhen_membership'];

  $title = $membership->label;

  if (redhen_membership_delete_multiple(array($membership->membership_id))) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $title)));
    watchdog('redhen_membership', 'Deleted membership %title.', array('%title' => $title), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $title)), 'error');
  }
}

/**
 * Returns HTML for a list of available membership types for membership creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_redhen_membership_add_list($variables) {
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
    $output = '<p>' . t('You have not created any membership types yet. Go to the <a href="@create-membership-types">membership type creation page</a> to add a new content type.', array('@create-membership-types' => url('admin/structure/redhen/membership-types'))) . '</p>';
  }
  return $output;
}
