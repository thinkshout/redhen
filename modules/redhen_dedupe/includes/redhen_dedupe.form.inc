<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Form to select dedupe options.
 */
function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // Load the entities we want to merge:
  $entity_ids = explode(',', $entity_ids);
  $entities = redhen_contact_load_multiple($entity_ids);

  // Initialize our table header:
  $table_header = array(t('Field Name'));
  $master_options = array();

  // Loop through the entities to build out our table headers and master
  // entity options:
  foreach ($entities as $ent_id => $entity) {
    $wrapper = entity_metadata_wrapper('redhen_contact', $entity);
    $updated = format_date($wrapper->updated->value(), 'short');
    $table_header[] = $master_options[$ent_id] = t('@name (Updated: !date)', array('!date' => $updated, '@name' => $wrapper->label()));

    $form['contact_previews'][$ent_id] = array(
      '#type' => 'container',
      'contact' => $wrapper->view(),
      '#states' => array(
        'visible' => array(
          ':input[name="master"]' => array('value' => $ent_id),
        ),
      ),
      '#weight' => 99,
    );
  }

  // Pass along the entity ID options to the form handler:
  $form_state['entities'] = $entities;

  // Form field to select a merge master entity.
  $form['master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#options' => $master_options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
  );

  $form['contact_previews'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
  );

  $form['contacts'] = array(
    '#theme' => 'redhen_dedupe_form_table',
    '#tree' => TRUE,
    '#header' => $table_header,
  );

  $properties = entity_get_all_property_info('redhen_contact');
  foreach ($properties as $name => $property) {
    if (!redhen_dedupe_property_mergeable($name, $property)) {
      continue;
    }

    $form['contacts'][$name] = array(
      '#type' => 'container',
    );
    $row = &$form['contacts'][$name];
    $row[] = array('#markup' => $property['label']);

    foreach ($entities as $ent_id => $entity) {
      $views_array = redhen_contact_view($entity);
      $entity_view = $views_array['redhen_contact'][$ent_id];
      $row[]['value'] = array(
        '#type' => 'radio',
        '#title' => render($entity_view[$name]),
        '#value' => $ent_id,
        '#attributes' => array(
          'name' => $name,
        ),
      );
    }
  }

  $form['dead_contact_fate'] = array(
    '#type' => 'radios',
    '#title' => t('What do you want to do with the old records?'),
    '#default_value' => 1,
    '#options' => array(0 => t('Delete'), 1 => t('Archive')),
  );

  $related_types = array();
  if (module_exists('redhen_note')) {
    $related_types['redhen_note'] = t('Notes');
  }
  if (module_exists('redhen_engagement')) {
    $related_types['redhen_engagement'] = t('Engagement Scores');
  }
  if (module_exists('redhen_membership')) {
    $related_types['redhen_membership'] = t('Memberships');
  }
  if (module_exists('redhen_relation')) {
    $related_types['relation'] = t('Relationships/Affiliations');
  }
  if (count($related_types) > 0) {
    $form['related_entities'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Move items attached to old records to Master record:'),
      '#options' => $related_types,
      '#default_value' => array_keys($related_types),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Merge'),
  );

  return $form;
}

/**
 * Submit handler for dedupe merge form.
 */
function redhen_dedupe_merge_form_submit($form, &$form_state) {
  $master_id = $form_state['values']['master'];
  $master = redhen_contact_load($master_id);
  $merge_status = redhen_dedupe_merge($master, $form_state['entities'], $form_state['values']['values'], $form_state['values']['related_entities'], $form_state['values']['dead_contact_fate']);
  if ($merge_status) {
    drupal_set_message('Contacts merged.');
  }
}


/**
 * Themes the field associations on a fieldmap edit form into a table.
 */
function theme_redhen_dedupe_form_table($variables) {
  $elements = $variables['elements'];

  // Build the rows array.
  $columns = isset($elements['#columns']) ? $elements['#columns'] : (isset($elements['#header']) ? array_keys($elements['#header']) : array());
  $rows = array();
  foreach (element_children($elements) as $child_key) {
    $child = & $elements[$child_key];
    $data = array();
    $row_columns = empty($columns) ? element_children($child) : $columns;
    foreach ($row_columns as $column) {
      if (isset($child[$column])) {
        $cell = array('data' => drupal_render($child[$column]));
        if (isset($child[$column]['#attributes'])) {
          foreach ($child[$column]['#attributes'] as $key => $value) {
            $cell[$key] = ($key == 'id') ? (is_array($value) ? array($value[0] . '-cell') : $value . '-cell') : $value;
          }
        }
        $data[] = $cell;
      }
    }
    $row = array('data' => $data);
    if (isset($child['#attributes'])) {
      foreach ($child['#attributes'] as $key => $value) {
        $row[$key] = $value;
      }
    }
    $rows[] = $row;
  }

  $config = array(
    'rows' => $rows,
  );
  if (isset($elements['#header'])) {
    $config['header'] = $elements['#header'];
  }
  if (isset($elements['#attributes']) && is_array($elements['#attributes'])) {
    $config['attributes'] = $elements['#attributes'];
  }

  return theme('table', $config);
}

/**
 * Determine if a given property can be merged.
 *
 * @param array $property
 *   A property as returned by entity_get_all_property_info();
 *
 * @return bool
 *   True is mergeable.
 */
function redhen_dedupe_property_mergeable($name, $property) {
  if (isset($property['computed']) && $property['computed']) {
    return FALSE;
  }

  $info = entity_get_info('redhen_contact');
  if (in_array($name, $info['entity keys'])) {
    return FALSE;
  }

  return TRUE;
}
