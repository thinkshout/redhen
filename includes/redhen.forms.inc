<?php

/**
 * @file
 * Defines forms shared across the Redhen CRM.
 */

/**
 * Returns the form to filter redhen entity lists.
 */
function redhen_filter_form($form, $form_state, $entity_type) {
  // add jquery libs and
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'redhen') . '/misc/redhen.forms.js');

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
    foreach ($property_info as $key => $prop) {
      if (isset($prop['filter']) && $prop['filter']) {
        $field_type = isset($prop['field_type']) ? $prop['field_type'] : $prop['type'];

        // conver datetime defaults to date strings
        $default_value = isset($_GET['properties'][$key]) ? $_GET['properties'][$key] : NULL;
        if ($prop['type'] == 'date' && !empty($default_value)) {
          $default_value['from'] = ($default_value['from'] == '') ? '' :
            date('m/d/Y', strtotime($default_value['from']));
          $default_value['to'] = ($default_value['to'] == '') ? '' :
            date('m/d/Y', strtotime($default_value['to']));
        }

        if ($prop['type'] == 'date') {
          $form['filter_fieldset']['filters']['properties'][$key]['from'] = array(
            '#type' => $field_type,
            '#title' => $prop['label'] . ' from',
            '#attributes' => array('class' => array("redhen-field-type-{$prop['type']}")),
            '#default_value' => $default_value['from']
          );
          $form['filter_fieldset']['filters']['properties'][$key]['to'] = array(
            '#type' => $field_type,
            '#title' => $prop['label'] . ' to',
            '#attributes' => array('class' => array("redhen-field-type-{$prop['type']}")),
            '#default_value' => $default_value['to']
          );
        }
        else {
          $form['filter_fieldset']['filters']['properties'][$key] = array(
            '#type' => $field_type,
            '#title' => $prop['label'],
            '#attributes' => array('class' => array("redhen-field-type-{$prop['type']}")),
            '#default_value' => $default_value
          );
        }

        if ($field_type == 'select' && isset($prop['options list'])) {
          $select_options = array('' => t('-- Any --')) + call_user_func($prop['options list']);
          $form['filter_fieldset']['filters']['properties'][$key]['#options'] = $select_options;
        }
      }
    }

    // add bundles; will load additional field filters on change
    $type_options = array('' => t('-- Any --'));
    $entity_info = entity_get_info($entity_type);
    foreach ($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }

    // single bundle, just load fields
    if (count($entity_info['bundles']) == 1) {
      $bundle = array_keys($entity_info['bundles']);
      $bundle = $bundle[0];
      redhen_filter_form_fields($form, $entity_type, $bundle);
    }
    else {
      $form['filter_fieldset']['filters']['bundle'] = array(
        '#type' => 'select',
        '#title' => t('@type types', array('@type' => $entity_info['label'])),
        '#options' => $type_options,
        '#default_value' => isset($_GET['bundle']) ? $_GET['bundle'] : NULL,
        '#ajax' => array(
          'callback' => 'redhen_filter_fields_callback',
          'wrapper' => 'redhen_filter_fields_container',
          'method' => 'replace',
          'effect' => 'fade',
          'progress' => array(
            'type' => 'throbber',
            'message' => t('Retrieving fields for this @entity_type type.',
              array('@entity_type' => $entity_info['label']))
          )
        )
      );
      $form['filter_fieldset']['filters']['fields'] = array(
        '#prefix' => '<div id="redhen_filter_fields_container">',
        '#suffix' => '</div>'
      );
    }

    $form['filter_fieldset']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Apply')
    );

    $form['filter_fieldset']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_filter_form_reset')
    );
  }

  // add the fields on an ajax type change event or if a filter is being passed
  if (!empty($form_state['values']['filters']['bundle']) || isset($_GET['bundle'])) {
    $bundle = !empty($form_state['values']['filters']['bundle']) ?
      $form_state['values']['filters']['bundle'] : $_GET['bundle'];

    redhen_filter_form_fields($form, $entity_type, $bundle);
  }

  return $form;
}

/**
 * AJAX callback to return fields for a given type.
 */
function redhen_filter_fields_callback($form, $form_state) {
  return $form['filter_fieldset']['filters']['fields'];
}

/**
 * Validation callback for filter form.
 *
 * @param $form
 * @param $form_state
 */
function redhen_filter_form_validate($form, &$form_state) {
  //Todo: Date validation?
}

/**
 * Submit handler for redhen_contact_filter_form().
 */
function redhen_filter_form_submit($form, &$form_state) {
  $filters = $form_state['values']['filters'];
  $form_state['redirect'] = array($_GET['q'], array('query' => $filters));
}

/**
 * Submit handler to reset the filter form.
 */
function redhen_filter_form_reset($form, &$form_state) {
  $form_state['redirect'] = array($_GET['q']);
}

/**
 * Populate a form array with field filters for given bundle.
 *
 * @param $form
 * @param $entity_type
 * @param $bundle
 */
function redhen_filter_form_fields(&$form, $entity_type, $bundle) {
  $fields = field_info_instances($entity_type, $bundle);
  $form['filter_fieldset']['filters']['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter by %bundle specific fields',
      array('%bundle' => $bundle)),
    '#collapsible' => FALSE,
    '#id' => 'redhen_filter_fields_container',
    '#access' => !empty($fields)
  );

  if (!empty($fields)) {
    // loop through each field adding a filter
    $filters_count = 0;
    foreach ($fields as $key => $field) {
      $field_info = field_info_field($field['field_name']);
      // for now, we're only allowing fields that have tid or value single cols
      // @todo: expand to allow more field types
      $columns = array_keys($field_info['columns']);
      $column = reset($columns);
      if (in_array($column, array('value', 'tid'))) {
        $filters_count++;
        $form['filter_fieldset']['filters']['fields'][$key][$column] = array(
          '#type' => 'textfield',
          '#title' => $field['label'],
          '#default_value' => isset($_GET['fields'][$key][$column]) ?
            $_GET['fields'][$key][$column] : NULL,
        );

        // override for select lists
        if ($field['widget']['type'] == 'options_select') {
          $options = array('' => t('-- Any --')) + $field_info['settings']['allowed_values'];
          $form['filter_fieldset']['filters']['fields'][$key][$column]['#type'] = 'select';
          $form['filter_fieldset']['filters']['fields'][$key][$column]['#options'] = $options;
        }
      }
    }

    if (!$filters_count) {
      $form['filter_fieldset']['filters']['fields']['message'] = array(
        '#markup' => t('There are no additional fields for %bundle.', array(
          '%bundle' => $bundle
        ))
      );
    }
  }
}
