<?php

/**
 * @file
 * Redhen enagagement score editing UI.
 */

/**
 * Generates the Redhen Contact type editing form.
 */
function redhen_engagement_score_form($form, &$form_state, $redhen_engagement_score, $op = 'edit') {
  if ($op == 'clone') {
    $redhen_engagement_score->label .= ' (cloned)';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $redhen_engagement_score->label,
    '#description' => t('The human-readable name of this engagement score.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['score'] = array(
    '#title' => t('Scpre'),
    '#type' => 'textfield',
    '#default_value' => $redhen_engagement_score->score,
    '#description' => t('The numeric value of this score.'),
    '#required' => TRUE,
    '#size' => 10,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save contact type'),
    '#weight' => 40,
  );

  if (!$redhen_engagement_score->locked && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete engagement score'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_engagement_score_form_submit_delete')
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function redhen_engagement_score_form_submit(&$form, &$form_state) {
  $redhen_engagement_score = entity_ui_form_submit_build_entity($form, $form_state);
  $redhen_engagement_score->save();
  $form_state['redirect'] = 'admin/structure/redhen/engagement_scores';
}

/**
 * Form API submit callback for the delete button.
 */
function redhen_engagement_score_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/redhen/contact_types/manage/' . $form_state['redhen_contact_type']->type . '/delete';
}
