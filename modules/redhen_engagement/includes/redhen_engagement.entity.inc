<?php

/**
 * @file
 * RedhenEngagement entity classes
 */

/**
 * The class used for note entities.
 */
class RedhenEngagement extends Entity {

  public
    $engagement_id = NULL,
    $type = 'engagement',
    $description,
    $engagement_score_id,
    $engagement_score,
    $contact_id,
    $source_entity_type,
    $source_entity_id,
    $source_entity,
    $author_uid,
    $author,
    $created;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_engagement');

    // New engagement. is_new might not be set so check for id
    if ($this->engagement_id) {
      $this->source_entity = $this->sourceEntity();
      $this->author = $this->author();
      $this->engagement_score = entity_load_single('redhen_engagement_score', $this->engagement_score_id);
    }
    else {
      $this->created = REQUEST_TIME;
    }
  }

  /**
   * Override label() to add note label.
   */
  public function label() {
    return 'Enagement ' . $this->engagement_id;
  }

  /*
   * Get entity note is attached to
   */
  public function sourceEntity() {
    $ret = NULL;
    if ($this->source_entity_type && $this->source_entity_id) {
      $ret = entity_load_single($this->source_entity_type, $this->source_entity_id);
    }
    return $ret;
  }

  /*
   * Get author of note
   */
  public function author() {
    return user_load($this->author_uid);
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/engagement/' . $this->engagement_id);
  }

}

/**
 * The class used for note entities.
 */
class RedhenEngagementScore extends Entity {

  public $name;
  public $label;
  public $locked;
  public $score;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_engagement_score');
  }

}

/**
 * Controls metadata for Redhen notes
 *
 * Redhen utilizes custom property attributes to determine if a property
 * should be available as a filter on the listing page. These attributes are
 *   filter: TRUE to add as a filter.
 *   field_type: textfield, select, etc.
 *   options: array of options for select field types.
 */
class RedhenEngagementMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the note was created."),
      'type' => 'date',
      'schema field' => 'created',
      'filter' => TRUE,
      'field_type' => 'textfield'
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the note."),
      'schema field' => 'author_uid'
    );
    $properties['engagement_score_id'] = array(
      'filter' => TRUE,
      'field_type' => 'select',
      'options list' => 'redhen_engagement_score_options'
    ) + $properties['type'];


    return $info;
  }

}

/**
 * UI Controller for Redhen engagement scores. Adds the score column.
 */
class RedhenEngagementScoreUIController extends EntityDefaultUIController {

  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $additional_header[] = t('Score');
    return parent::overviewTableHeaders($conditions, $rows, $additional_header);
  }

  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $additional_cols[] = $entity->score;
    return parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
  }

}
