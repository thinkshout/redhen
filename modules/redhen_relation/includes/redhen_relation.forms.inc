<?php

/**
 * @file
 * Form definition and handling for redhen relations.
 */

/**
 * Return a form array for adding/editing a connection.
 *
 * @param $form
 * @param $form_state
 * @param RedhenContact $contact
 * @param stdClass $relation
 * @return array
 */
function redhen_relation_connection_form($form, &$form_state, RedhenContact $contact, $relation = NULL) {
  $relation_types = relation_get_available_types('redhen_contact', $contact->bundle());

  // instantiate new relation
  if (!$relation) {
    $relation = relation_create(reset($relation_types)->relation_type, array());
  }

  // store contact and relation entities for use on submit
  $form_state['contact'] = $contact;
  $form_state['relation'] = $relation;

  $options = array();
  foreach($relation_types as $type) {
    $options[$type->relation_type] = $type->label;
  }
  $form['relation_type'] = array(
    '#title' => t('Connection type'),
    '#type' => 'select',
    '#options' => $options,
    '#default value' => isset($relation) ? $relation->relation_type : NULL
  );

  $options = array();
  foreach(entity_load('redhen_org') as $org) {
    $options[$org->org_id] = $org->label;
  }
  $form['org'] = array(
    '#title' => t('Organization'),
    '#type' => 'select',
    '#options' => $options
  );

  // attach any fields
  field_attach_form('relation', $relation, $form, $form_state);

  // hide the endpoints field widget. @TODO: Find out why appearing.
  $form['endpoints']['#access'] = FALSE;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save connection'),
    '#weight' => 999
  );

  return $form;
}

/**
 * Validation handler for redhen_relation_connection_form().
 */
function redhen_relation_connection_form_validate($form, &$form_state) {
  $contact = $form_state['contact'];
  $relation = $form_state['relation'];

  // set the endpoints field to the selected values.
  $endpoints =array (
    array('entity_type' => 'redhen_contact', 'entity_id' => $contact->contact_id),
    array('entity_type' => 'redhen_org', 'entity_id' => $form_state['values']['org']),
  );
  $form_state['values']['endpoints'][LANGUAGE_NONE] = $endpoints;

  // set the relation type based on selected value before validating
  $relation->relation_type = $form_state['values']['relation_type'];

  field_attach_form_validate('relation', $relation, $form, &$form_state);
}

/**
 * Submit handler for redhen_relation_connection_form().
 */
function redhen_relation_connection_form_submit($form, &$form_state) {
  $contact = $form_state['contact'];
  $relation = $form_state['relation'];

  $relation->relation_type = $form_state['values']['relation_type'];

  field_attach_submit('relation', $relation, $form, $form_state);
  if (relation_save($relation)) {
    drupal_set_message(t('The connection has been saved.'));
    $form_state['redirect'] = "redhen/contact/{$contact->contact_id}/connections";
  }
  else {
    drupal_set_message(t('The connection could not be saved.'), 'error');
  }
}
