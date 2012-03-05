<?php

/**
 * @file
 * Form definition and handling for redhen notes.
 */

function redhen_note_add($form, &$form_state, $entity, $entity_type) {
  $redhen_note = entity_get_controller('redhen_note')->create(array('type' => 'redhen_note'));
  return drupal_get_form('redhen_org_org_form', $org);
}

/**
 * Return a form array for adding/editing a connection.
 *
 * @param $form
 * @param $form_state
 * @param RedhenContact|RedhenOrg $entity
 * @param stdClass $relation
 * @return array
 */
function redhen_note_form($form, &$form_state, $entity, $entity_type, $redhen_note) {

  $form_state['redhen_note'] = $redhen_note;


  $form_state['entity'] = $entity;
  $form_state['entity_type'] = $entity_type;

  field_attach_form('redhen_note', $redhen_note, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Note'),
    '#weight' => 999,
  );

  return $form;
}

/**
 * Validation handler for redhen_note_form().
 */
function redhen_note_form_validate($form, &$form_state) {
  $redhen_note = $form_state['redhen_note'];
  field_attach_form_validate('redhen_note', $redhen_note, $form, &$form_state);
}

/**
 * Submit handler for redhen_note_form().
 */
function redhen_note_form_submit($form, &$form_state) {
  $redhen_note = $form_state['redhen_note'];
  $entity = $form_state['entity'];
  $entity_type = $form_state['entity_type'];

  $redhen_note->entity_type = $entity_type;

  list($entity_id,,) = entity_extract_ids($entity_type, $entity);
  $redhen_note->entity_id = $entity_id;

  field_attach_submit('redhen_note', $redhen_note, $form, $form_state);
  $redhen_note->save();
  drupal_set_message(t('The note has been saved.'));
  $uri = entity_uri($entity_type, $entity);
  $form_state['redirect'] = $uri['path'] . "/notes";
}
