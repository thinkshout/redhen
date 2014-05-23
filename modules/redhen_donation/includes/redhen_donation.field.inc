<?php

/**
 * @file
 * Field hooks.
 */

/**
 * Implements hook_field_info().
 */
function redhen_donation_field_info() {
  return array(
    'redhen_donation' => array(
      'label' => t('Donation'),
      'description' => t('Enables donations of a selected type for an entity.'),
      'settings' => array(),
      'instance_settings' => array(
        'default_redhen_donation_settings' => array(
          'status' => 0,
          'scheduling' => array(
            'open' => NULL,
            'close' => NULL,
          ),
          'settings' => array(
          ),
        ),
      ),
      'default_widget' => 'redhen_dontation_select',
      'default_formatter' => 'redhen_donation_form',
      'no_ui' => FALSE,
    ),
  );
}

/**
 * Implements hook_field_instance_settings_form().
 *
 * Add default donation field instance settings.
 */
function redhen_donation_field_instance_settings_form($field, $instance) {
  $form = $form_state = array();
  $default_settings = isset($instance['settings']['default_redhen_donation_settings']) ? $instance['settings']['default_redhen_donation_settings'] : array();

  // Flatten scheduling and reminder settings since this form is in tree mode.
  foreach ($default_settings as $key => $val) {
    if ($key != 'settings' and is_array($val)) {
      foreach ($val as $key1 => $val1) {
        if (is_array($val1)) {
          foreach ($val1 as $key2 => $val2) {
            $default_settings[$key2] = $val2;
          }
        }
        else {
          $default_settings[$key1] = $val1;
        }
      }
      unset($default_settings[$key]);
    }
  }

  $settings_form = redhen_donation_entity_settings_form($form, $form_state, $default_settings);

  $form['default_redhen_donation_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default donation settings'),
    '#collapsible' => TRUE,
    '#description' => t('These settings will be applied when an entity with this field is saved and does not yet have it\'s own settings applied.')
  );

  // Unset the save button just in case.
  unset($settings_form['save']);
  $form['default_redhen_donation_settings'] += $settings_form;

  return $form;
}

/**
 * Implements hook_field_validate().
 */
function redhen_donation_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  // @TODO: validation
}

/**
 * Implements hook_field_is_empty().
 */
function redhen_donation_field_is_empty($item, $field) {
  if (empty($item['redhen_donation_type']) && (string) $item['redhen_donation_type'] !== '') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_field_widget_info().
 */
function redhen_donation_field_widget_info() {
  return array(
    'redhen_donation_select' => array(
      'label' => t('Donation type'),
      'field types' => array('redhen_donation'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function redhen_donation_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $options = array('' => t('-- Disable Donations --'));
  foreach (redhen_donation_get_types() as $type) {
    $options[$type->name] = $type->label;
  }
  $element += array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($items[$delta]) ? $items[$delta] : array(),
  );

  // force some help text into the field, appending anything the user added.
  $element['#description'] .= ' ' . t('Select what type of donation should be
    enabled for this @type.', array('@type' => $instance['bundle']));

  return array('redhen_donation_type' => $element);
}

/**
 * Implements hook_field_formatter_info().
 */
function redhen_donation_field_formatter_info() {
  return array(
    'redhen_donation_default' => array(
      'label' => t('Default'),
      'field types' => array('redhen_donation'),
      'settings' => array(
        'label' => '',
      ),
    ),
    'redhen_donation_link' => array(
      'label' => t('Donation Link'),
      'field types' => array('redhen_donation'),
      'settings' => array(
        'label' => '',
      ),
    ),
    'redhen_donation_form' => array(
      'label' => t('Donation Form'),
      'field types' => array('redhen_donation'),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function redhen_donation_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();

  if ($display['type'] == 'redhen_donation_default' || $display['type'] == 'redhen_donation_link') {
    $element['label'] = array(
      '#title' => t('Label'),
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => $settings['label'],
      '#required' => FALSE,
      '#description' => t("Optional label to use when displaying the donation title or link. Leave blank to use the parent item's label."),
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function redhen_donation_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';

  if ($display['type'] == 'redhen_donation_default' || $display['type'] == 'redhen_donation_link') {
    if (!empty($settings['label'])) {
      $summary = t('Donation label: @label.', array('@label' => $settings['label']));
    }
    else {
      $summary = t('Donation label: Parent label.');
    }
  }

  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function redhen_donation_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  // we know we should only have a single item
  if (isset($items[0]['redhen_donation_type']) && !empty($items[0]['redhen_donation_type'])) {
    $donation_type = redhen_donation_type_load($items[0]['redhen_donation_type']);
    $settings = $display['settings'];
    $label = !empty($settings['label']) ? $settings['label'] : $donation_type->label;

    switch ($display['type']) {
      case 'redhen_donation_default':
        $element[0] = array('#markup' => $label);
        break;
      case 'redhen_donation_link':
        // Enable donation link if accessible.
        if (redhen_donation_donate_page_access($entity_type, $entity)) {
          $uri = entity_uri($entity_type, $entity);
          $element[0] = array(
            '#markup' => theme('redhen_donation_link',
              array(
                'label' => $label,
                'path' => $uri['path'] . '/donate'
              )
            )
          );
        }
        break;
      case 'redhen_donation_form':
        // Enable donation form if accessible.
        list($entity_id) = entity_extract_ids($entity_type, $entity);
        if (redhen_donation_donate_page_access($entity_type, $entity) && redhen_donation_status($entity_type, $entity_id)) {
          $donation = entity_get_controller('redhen_donation')->create(array(
            'entity_type' => $entity_type,
            'entity_id' => $entity_id,
            'type' => $donation_type->name,
          ));
          $element[0] = drupal_get_form('redhen_donation_form', $donation);
        }
        break;
    }
  }

  return $element;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Hide the cardinality setting on the field settings for donation fields.
 */
function redhen_donation_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  if ($form['#field']['type'] == 'redhen_donation') {
    $form['field']['cardinality']['#default_value'] = 1;
    $form['field']['cardinality']['#access'] = FALSE;
    $form['#validate'][] = 'redhen_donation_form_field_ui_field_edit_form_validate';
  }
}

/**
 * Validation handler for redhen_donationdrush c_form_field_ui_field_edit_form.
 *
 * Ensure cardinality is set to 1 on donation fields.
 *
 * @param $form
 * @param $form_state
 */
function redhen_donation_form_field_ui_field_edit_form_validate(&$form, &$form_state) {
  if ($form['#field']['type'] == 'redhen_donation') {
    if ($form_state['values']['field']['cardinality'] !== 1) {
      form_set_error('cardinality', t('Cardinality on donation fields must be set to one.'));
    }

    // Validate default donation settings.
    $default_settings = $form_state['values']['instance']['settings']['default_redhen_donation_settings'];
    $base_elem_key = 'instance][settings][default_redhen_donation_settings][';

    // Validate open date.
    if (!empty($default_settings['scheduling']['open']) && strtotime($default_settings['scheduling']['open']) === FALSE) {
      form_set_error($base_elem_key . 'scheduling][open', t('Date is invalid.'));
    }

    // Validate close date.
    if (!empty($default_settings['scheduling']['close']) && strtotime($default_settings['scheduling']['close']) === FALSE) {
      form_set_error($base_elem_key . 'scheduling][close', t('Date is invalid.'));
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add a validation handler for redhen_donation_form_field_ui_field_overview_form().
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function redhen_donation_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
  $form['#validate'][] = 'redhen_donation_form_field_ui_field_overview_form_validate';
}

/**
 * Validation callback for redhen_donation_form_field_ui_field_overview_form().
 *
 * Ensure only one donation field is added per entity.
 *
 * @param $form
 * @param $form_state
 */
function redhen_donation_form_field_ui_field_overview_form_validate(&$form, &$form_state) {
  $fields = $form_state['values']['fields'];
  if ($fields['_add_new_field']['type'] == 'redhen_donation') {
    foreach ($form['#fields'] as $field_name) {
      $field = field_info_field($field_name);
      if ($field['type'] == 'redhen_donation') {
        form_set_error('_add_new_field', t('An entity can only have one donation field.'));
      }
    }
  }
}

/**
 * Implements hook_field_create_instance().
 */
function redhen_donation_field_create_instance($instance) {
  // Rebuild menu to recognize donation paths.
  _redhen_donation_menu_rebuild($instance);
}

/**
 * Implements hook_field_delete_instance().
 */
function redhen_donation_field_delete_instance($instance) {
  // Remove donation paths from menu router.
  _redhen_donation_menu_rebuild($instance);

  // @TODO: should we delete all donations at this point?
}

/**
 * Rebuild the menu for a given donation field instance.
 *
 * @param $instance
 *   Field instance being added or deleted.
 */
function _redhen_donation_menu_rebuild($instance) {
  $donation_fields = field_read_fields(array('type' => 'redhen_donation'));
  if (in_array($instance['field_name'], array_keys($donation_fields))) {
    menu_rebuild();
  }
}
