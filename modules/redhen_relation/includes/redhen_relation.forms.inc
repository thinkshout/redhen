<?php

/**
 * @file
 * Form definition and handling for redhen relations.
 */

/**
 * Return a form array for adding/editing a connection.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 * @param RedhenContact|RedhenOrg $entity
 *   Entity object.
 *
 * @return mixed
 *   Form array.
 */
function redhen_relation_connection_form($form, &$form_state, $entity) {
  $relation_types = redhen_relation_get_available_types($entity->entityType(), $entity->bundle(), 'both');

  // There are no valid relation types for this entity, so exit w/message.
  if (empty($relation_types)) {
    $form['message'] = array(
      '#markup' => t('%label has no valid relation types so a connection cannot be made.', array('%label' => $entity->label())),
    );
    return $form;
  }

  // Instantiate new relation based on default or submitted relation type.
  $relation_type = isset($form_state['values']['relation_type']) ?
    $form_state['values']['relation_type'] :
    reset($relation_types)->relation_type;
  $relation = relation_create($relation_type, array());

  // Store contact and relation entities for use on submit.
  $form_state['entity'] = $entity;
  $form_state['relation'] = $relation;

  // Load all available relation types.
  $options = array();
  foreach ($relation_types as $type) {
    list($endpoint_entity_type) = explode(':', $type->source_bundles[0]);
    $reverse = $type->directional & ($endpoint_entity_type == $entity->entityType());

    $options[$type->relation_type] = relation_get_type_label($type, $reverse);
  }
  $form['relation_type'] = array(
    '#title' => t('Connection type'),
    '#type' => 'select',
    '#options' => $options,
    '#default value' => isset($relation) ? $relation->relation_type : NULL,
    '#ajax' => array(
      'callback' => 'redhen_relation_form_refresh',
      'wrapper' => 'redhen_relation_fields',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'throbber',
        'message' => t('Retrieving fields for this connection type.'),
      ),
    ),
    '#description' => t('Connections are only allowed between contacts and organizations.'),
  );

  // Determine the entity type we're going to relate to.
  $active_relation_type = $relation_types[$relation_type];
  $entity_type_to_relate = '';
  if (!empty($active_relation_type->target_bundles)) {
    list($tgt_entity_type) = explode(':', $active_relation_type->target_bundles[0]);
    if ($entity->entityType() != $tgt_entity_type) {
      $entity_type_to_relate = $tgt_entity_type;
    }
  }

  if (empty($entity_type_to_relate)) {
    list($entity_type_to_relate) = explode(':', $active_relation_type->source_bundles[0]);
  }

  $form_state['entity_to_relate_type'] = $entity_type_to_relate;

  $form['fields'] = array(
    '#type' => 'fieldset',
    '#id' => 'redhen_relation_fields',
  );
  // Attach any fields.
  field_attach_form('relation', $relation, $form['fields'], $form_state);

  $info = entity_get_info($entity_type_to_relate);
  $plural_label = isset($info['plural label']) ? $info['plural label'] : $info['label'] . 's';

  $form['fields']['new_or_exsiting'] = array(
    '#type' => 'select',
    '#options' => array(
      'Existing',
      'New',
    ),
    '#weight' => 9,
  );

  $form['fields']['entity_to_relate'] = array(
    '#title' => $plural_label,
    '#type' => 'textfield',
    '#required' => ($active_relation_type->min_arity == 1) ? FALSE : TRUE,
    '#access' => ($active_relation_type->max_arity == 1) ? FALSE : TRUE,
    '#autocomplete_path' => 'redhen/relation/autocomplete/' . $relation->relation_type . '/' . $entity_type_to_relate . '/' . $entity->entityType() . '/' . $entity->internalIdentifier(),
    '#states' => array(
      'invisible' => array(
        ':input[name="new_or_exsiting"]' => array(
          'value' => '1',
        ),
      ),
      'required' => array(
        ':input[name="new_or_exsiting"]' => array(
          'value' => '0',
        ),
      ),
    ),
    '#weight' => 10,
  );

  $form['entity_info'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'invisible' => array(
        ':input[name="new_or_exsiting"]' => array(
          'value' => '0',
        ),
      ),
    ),
    '#weight' => 9,
  );

  // Get entity callbacks and variables.
  $entity_types = array();
  switch ($entity_type_to_relate) {
    case 'redhen_contact':
      module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');
      $entity_types = redhen_contact_get_types();
      $create_function = 'redhen_contact_create';
      $entity_form_callback = 'redhen_contact_contact_form';
      break;

    case 'redhen_org':
      module_load_include('inc', 'redhen_org', 'includes/redhen_org.forms');
      $entity_types = redhen_org_get_types();
      $create_function = 'redhen_org_create';
      $entity_form_callback = 'redhen_org_org_form';
      break;

  }

  $entity_type_options = array('_none' => 'None');
  foreach ($entity_types as $type_name => $type) {
    $entity_type_options[$type_name] = $type->label;
  }
  $form['entity_info']['entity_type'] = array(
    '#type' => 'select',
    '#title' => t('Select type'),
    '#options' => $entity_type_options,
    '#ajax' => array(
      'callback' => 'redhen_relation_form_refresh',
      'wrapper' => 'entity-form',
    ),
    '#id' => 'entity-type',
    '#states' => array(
      'required' => array(
        ':input[name="new_or_exsiting"]' => array(
          'value' => '1',
        ),
      ),
    ),
  );

  $form['entity_info']['entity_form_wrapper'] = array(
    '#type' => 'container',
  );

  $entity_type = NULL;
  if ($form_state['input'] && $form_state['input']['_triggering_element_name'] !== 'relation_type' && $form_state['input']['entity_type'] != '_none') {
    $entity_type = $form_state['input']['entity_type'];
  }

  if (!is_null($entity_type)) {
    $target_entity = $create_function(array('type' => $entity_type));
    $entity_form = $entity_form_callback(array(), $form_state, $target_entity);
    unset($entity_form['actions']);
  }
  else {
    $entity_form = array();
  }

  $entity_form['#prefix'] = '<div id="entity-form">';
  $entity_form['#suffix'] = '</div>';
  $form['entity_info']['entity_form_wrapper']['form'] = $entity_form;

  // Hide the endpoints field widget. @TODO: Find out why appearing.
  $form['fields']['endpoints']['#access'] = FALSE;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save connection'),
    '#weight' => 29,
  );

  return $form;
}

/**
 * Relation form AJAX callback.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 *
 * @return array
 *   AJAX commands.
 */
function redhen_relation_form_refresh(&$form, &$form_state) {
  $html1 = drupal_render($form['entity_info']['entity_form_wrapper']);
  $html2 = drupal_render($form['fields']);
  unset($form['entity_info']['entity_type']['#title']);
  $html3 = drupal_render($form['entity_info']['entity_type']);
  $result = array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace('#entity-form', $html1),
      ajax_command_replace('#redhen_relation_fields', $html2),
      ajax_command_replace('#entity-type', $html3),
    ),
  );
  return $result;
}

/**
 * Validation handler for redhen_relation_connection_form().
 */
function redhen_relation_connection_form_validate($form, &$form_state) {
  $entity = $form_state['entity'];
  $relation = $form_state['relation'];

  if ($form_state['input']['new_or_exsiting'] == 0 && isset($form_state['values']['entity_to_relate'])) {
    // Load the relation information, to determine arity.
    $relation_type = relation_type_load($form_state['values']['relation_type']);

    // parse out the entity id from the autocomplete string
    preg_match('/(.+) \((\d+)\)$/', $form_state['values']['entity_to_relate'], $matches);

    if (isset($matches[2]) && ($matches[2] > 0)) {
      list($src_relation_entity_type) = explode(":", $relation_type->source_bundles[0]);

      // ensure source/target order are correct
      if ($relation_type->directional && $form_state['entity_to_relate_type'] == $src_relation_entity_type) {
        $endpoints = array(
          array(
            'entity_type' => $form_state['entity_to_relate_type'],
            'entity_id' => $matches[2]
          ),
          array(
            'entity_type' => $entity->entityType(),
            'entity_id' => $entity->internalIdentifier()
          )
        );
      }
      else {
        $endpoints = array(
          array(
            'entity_type' => $entity->entityType(),
            'entity_id' => $entity->internalIdentifier()
          ),
          array(
            'entity_type' => $form_state['entity_to_relate_type'],
            'entity_id' => $matches[2]
          )
        );
      }

      $form_state['values']['endpoints'][LANGUAGE_NONE] = $endpoints;
    }
    else if ($relation_type->min_arity == 1 && empty($form_state['values']['entity_to_relate'])) {
      $endpoints = array(
        array(
          'entity_type' => $entity->entityType(),
          'entity_id' => $entity->internalIdentifier()
        )
      );
      $form_state['values']['endpoints'][LANGUAGE_NONE] = $endpoints;
    }
    else {
      form_set_error('entity_to_relate', 'Invalid connection.');
    }

    // Set the relation type based on selected value before validating.
    $relation->relation_type = $form_state['values']['relation_type'];
  }
  field_attach_form_validate('relation', $relation, $form, $form_state);

  // Check type if required.
  if ($form_state['input']['new_or_exsiting'] == 1 && $form_state['values']['entity_type'] == '_none') {
    form_set_error('entity_type', 'Entity type is required.');
  }
}

/**
 * Submit handler for redhen_relation_connection_form().
 */
function redhen_relation_connection_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];
  $relation = $form_state['relation'];

  if ($form_state['input']['new_or_exsiting'] == 1) {
    $entity_to_relate = $form_state[$form_state['entity_to_relate_type']];
    field_attach_submit($form_state['entity_to_relate_type'], $entity_to_relate, $form, $form_state);
    // Attach properties.
    $entity_properties = entity_get_property_info($form_state['entity_to_relate_type']);
    foreach ($entity_properties['properties'] as $entity_property => $data) {
      if (isset($form_state['values'][$entity_property])) {
        $entity_to_relate->$entity_property = $form_state['values'][$entity_property];
      }
    }
    entity_save($entity_to_relate, $entity_to_relate);
    list($entity_to_relate_id, ,) = entity_extract_ids($form_state['entity_to_relate_type'], $entity_to_relate);

    // Set relation.
    $endpoints = redhen_relation_endpoints(
      $form_state['values']['relation_type'],
      $form_state['entity_to_relate_type'],
      $entity_to_relate_id,
      $entity
    );

    $form_state['values']['endpoints'][LANGUAGE_NONE] = $endpoints;
  }

  $relation->relation_type = $form_state['values']['relation_type'];

  field_attach_submit('relation', $relation, $form, $form_state);
  if (relation_save($relation)) {
    drupal_set_message(t('The connection has been saved.'));
    $uri = entity_uri($entity->entityType(), $entity);
    $form_state['redirect'] = $uri['path'] . "/connections";
  }
  else {
    drupal_set_message(t('The connection could not be saved.'), 'error');
  }
}

/**
 * Set relation endpoints helper function.
 *
 * @param string $relation_type
 *   Type or relation.
 * @param string $entity_to_relate_type
 *   Related entity type.
 * @param object $entity_id
 *   Entity ID.
 * @param object $entity
 *   Entity.
 *
 * @return array
 *   Relation endpoints.
 */
function redhen_relation_endpoints($relation_type, $entity_to_relate_type, $entity_id, $entity) {
  $relation_type = relation_type_load($relation_type);
  list($src_relation_entity_type) = explode(":", $relation_type->source_bundles[0]);

  // Ensure source/target order are correct.
  if ($relation_type->directional && $entity_to_relate_type == $src_relation_entity_type) {
    $endpoints = array(
      array(
        'entity_type' => $entity_to_relate_type,
        'entity_id' => $entity_id,
      ),
      array(
        'entity_type' => $entity->entityType(),
        'entity_id' => $entity->internalIdentifier(),
      ),
    );
  }
  else {
    $endpoints = array(
      array(
        'entity_type' => $entity->entityType(),
        'entity_id' => $entity->internalIdentifier(),
      ),
      array(
        'entity_type' => $entity_to_relate_type,
        'entity_id' => $entity_id,
      ),
    );
  }
  return $endpoints;
}
