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
    foreach($property_info as $key => $prop) {
      if (isset($prop['filter']) && $prop['filter']) {
        $field_type = isset($prop['field_type']) ? $prop['field_type'] : $prop['type'];

        // conver datetime defaults to date strings
        $default_value = isset($_GET['properties'][$key]) ? $_GET['properties'][$key] : NULL;
        if ($prop['type'] == 'date' && !empty($default_value)) {
          $default_value['from'] = ($default_value['from'] == '') ? '' :
            date('m/d/Y', strtotime($default_value['from']));
          $default_value['to'] = ($default_value['to'] == '') ? '' :
            date('m/d/Y', strtotime($default_value['to']));;
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

        if ($field_type == 'select' && isset($prop['options'])) {
          $prop['options'] = array('' => t('-- Any --')) + $prop['options'];
          $form['filter_fieldset']['filters']['properties'][$key]['#options'] = $prop['options'];
        }
      }
    }

    // add bundles; will load additional field filters on change
    $type_options = array('' => t('-- Any --'));
    $entity_info = entity_get_info($entity_type);
    foreach($entity_info['bundles'] as $key => $bundle) {
      $type_options[$key] = $bundle['label'];
    }

    if (count($entity_info['bundles']) == 1) {
      $bundle = array_keys($entity_info['bundles']);
      $bundle = $bundle[0];
      $fields = field_info_instances($entity_type, $bundle);
      if (!empty($fields)) {
        $form['filter_fieldset']['filters']['fields'] = array(
          '#type' => 'fieldset',
          '#title' => t('Filter by @entity_type type specific fields',
            array('@entity_type' => drupal_strtolower($entity_info['label']))),
          '#collapsible' => FALSE,
          '#id' => 'redhen_filter_fields_container'
        );

        foreach($fields as $key => $field) {
          $form['filter_fieldset']['filters']['fields'][$key] = array(
            '#type' => 'textfield',
            '#title' => $field['label'],
            '#default_value' => isset($_GET['fields'][$key]) ?
              $_GET['fields'][$key] : NULL,
          );
        }
      }
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
              array('@entity_type' => drupal_strtolower($entity_info['label'])))
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
    $entity_info = entity_get_info($entity_type);
    $bundle = !empty($form_state['values']['filters']['bundle']) ?
      $form_state['values']['filters']['bundle'] : $_GET['bundle'];

    $fields = field_info_instances($entity_type, $bundle);
    if (!empty($fields)) {
      $form['filter_fieldset']['filters']['fields'] = array(
        '#type' => 'fieldset',
        '#title' => t('Filter by @entity_type type specific fields',
          array('@entity_type' => drupal_strtolower($entity_info['label']))),
        '#collapsible' => FALSE,
        '#id' => 'redhen_filter_fields_container'
      );

      foreach($fields as $key => $field) {
        $form['filter_fieldset']['filters']['fields'][$key] = array(
          '#type' => 'textfield',
          '#title' => $field['label'],
          '#default_value' => isset($_GET['fields'][$key]) ?
            $_GET['fields'][$key] : NULL,
        );
      }
    }
  }

  return $form;
}

/**
 * AJAX callback to return fields for a given type.
 */
function redhen_filter_fields_callback($form, $form_state) {
  // @TODO - Allow filtering on addressfields. Removed below b/c they don't work with EFQs.
  if (!empty($form['filter_fieldset']['filters']['fields']['field_address'])) {
    unset($form['filter_fieldset']['filters']['fields']['field_address']);
  }
  return $form['filter_fieldset']['filters']['fields'];
}

function redhen_filter_form_validate($form, &$form_state) {
  foreach ($form_state['values']['filters']['properties'] as $key => $property) {
    //Todo: Date validation?
//    if (!empty($property) && $form['filter_fieldset']['filters']['properties'][$key]['#attributes']['class'][0] == 'redhen-field-type-date') {
//
//      if ( $time = strtotime($property)) {
//        $form_state['values']['filters']['properties'][$key] = $time;
//      }
//      else {
//        form_set_error($key, t('@key is not a valid date.', array('@key' => $key)));
//      }
//    }
  }
}

/**
 * Submit handler for redhen_contact_filter_form().
 */
function redhen_filter_form_submit($form, &$form_state) {
  $filters = $form_state['values']['filters'];
  drupal_goto($_GET['q'], array('query' => $filters));
}

/**
 * Submit handler to reset the filter form.
 */
function redhen_filter_form_reset() {
  drupal_goto($_GET['q']);
}
