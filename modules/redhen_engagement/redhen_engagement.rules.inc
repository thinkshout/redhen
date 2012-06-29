<?php

/**
 * @file
 * Rules integration for RedHen Engagements.
 */

/**
 * Implements hook_rules_action_info().
 */
function redhen_engagement_rules_action_info() {
  $defaults = array(
    'parameter' => array(
      'score' => array(
        'type' => 'list<integer>',
        'label' => t('Engagement Score'),
        'options list' => 'redhen_engagement_score_options',
      ),
    ),
    'group' => t('Engagement'),
    'access callback' => 'redhen_engagement_access',
  );

  // registration
  if (module_exists('registration')) {
    $actions['score_registration_engagement'] = $defaults + array(
      'label' => t('Score registration engagement'),
      'base' => 'redhen_engagement_rules_action_score_registration',
    );
    $actions['score_registration_engagement']['parameter']['registration'] = array(
      'type' => 'registration',
      'label' => t('Registration'),
      'description' => t('The registration entity to be scored.'),
    );
  }

  // comment
  if (module_exists('comment')) {
    $actions['score_comment_engagement'] = $defaults + array(
      'label' => t('Score comment engagement'),
      'base' => 'redhen_engagement_rules_action_score_comment',
    );
    $actions['score_comment_engagement']['parameter']['comment'] = array(
      'type' => 'comment',
      'label' => t('Comment'),
      'description' => t('The comment entity to be scored.'),
    );
  }

  // webform
  if (module_exists('webform_rules')) {
    $actions['score_webform_engagement'] = $defaults + array(
      'label' => t('Score webform engagement'),
      'base' => 'redhen_engagement_rules_action_score_webform',
    );
    $actions['score_webform_engagement']['parameter']['webform'] = array(
      'type' => 'webform',
      'label' => t('Webform'),
      'description' => t('The webform submission to be scored. <em>Note: At this time scoring of anonymous webform submissions is not supported.</em>'),
    );
  }

  return $actions;
}

/**
 * Helper function for when the "registration engagement" Rule action is triggered.
 */
function redhen_engagement_rules_action_score_registration($scores, $registration) {
  if (!empty($registration->anon_mail)) {
    $contact = redhen_engagement_rules_action_find_by_mail($registration->anon_mail);
  }
  else {
    if (!empty($registration->user_uid)) {
      $contact = redhen_contact_load_by_user(user_load($registration->user_uid));
    }
  }

  if ($contact) {
    redhen_enagement_rules_action_score_contact($scores, $contact, t('Scored registration'), 'registration', $registration->registration_id);
  }
}

/**
 * Helper function for when the "comment enagement" Rule action is triggered.
 */
function redhen_engagement_rules_action_score_comment($scores, $comment) {
  if (!empty($comment->mail)) {
    $contact = redhen_engagement_rules_action_find_by_mail($comment->mail);
  }
  else {
    if (!empty($comment->uid)) {
      $contact = redhen_contact_load_by_user(user_load($comment->uid));
    }
  }

  if ($contact) {
    redhen_enagement_rules_action_score_contact($scores, $contact, t('Scored comment'), 'comment', $comment->cid);
  }
}

/**
 * Helper function for when the "webform enagement" Rule action is triggered.
 */
function redhen_engagement_rules_action_score_webform($score, $webform) {
  $result = db_query('SELECT w.nid, w.uid FROM {webform_submissions} w WHERE w.sid = :sid', array(':sid' => $webform['sid']));
  foreach ($result as $record) {
    if ($record->uid != 0) {
      $contact = redhen_contact_load_by_user(user_load($record->uid));
      $nid = $record->nid;
    }
  }
  ;

  if ($contact) {
    redhen_enagement_rules_action_score_contact($score, $contact, t('Scored webform submission'), 'node', $nid);
  }
}

/**
 * Helper function for finding a contact by an email address.
 */
function redhen_engagement_rules_action_find_by_mail($email) {
  $query = new EntityFieldQuery;
  $query
    ->entityCondition('entity_type', 'redhen_contact')
    ->propertyCondition('redhen_state', REDHEN_STATE_ACTIVE)
    ->fieldCondition('redhen_contact_email', 'value', $email, '=');
  $result = $query->execute();
  if (!empty($result)) {
    $contact = redhen_contact_load(reset(array_keys($result['redhen_contact'])));
    return $contact;
  }
  else {
    return FALSE;
  }
}

/**
 * Helper function for saving a comment engagement score when the Rule action is triggered.
 */
function redhen_enagement_rules_action_score_contact($score_ids, $contact, $description = '', $entity_type, $entity_id) {
  foreach ($score_ids as $id) {
    $score = entity_load_single('redhen_engaement', $id);
    $engagement = redhen_engagement_create($score->name, $contact->contact_id, $description, $entity_type, $entity_id);
    redhen_engagement_save($engagement);
  }
}
