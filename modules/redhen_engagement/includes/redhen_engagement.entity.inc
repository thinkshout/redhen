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
    $engagement_score,
    $contact_id,
    $source_entity_type,
    $source_entity_id,
    $author_uid,
    $created;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_engagement');

    // New engagement. is_new might not be set so check for id
    if (!$this->engagement_id) {
      $this->created = REQUEST_TIME;
    }
  }

  /**
   * Override label() to add note label.
   */
  public function label() {
    return t('Engagement !id', array('!id' => $this->engagement_id));
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
 *   filter_operator: EFQ supported operators. Defaults to = or IN depending on value submitted
 *   field_type: textfield, select, etc.
 *   options list: callback that returns the options for this field.
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
      // RedHen specific attributes
      'filter' => TRUE,
      'field_type' => 'textfield',
    );

    // Entities

    $properties['entity'] = array(
      'label' => t("Source entity"),
      'type' => 'entity',
      'schema field' => 'entity',
      'description' => t("The host entity."),
      'getter callback' => 'redhen_engagement_property_source_get',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['contact'] = array(
      'label' => t("Contact"),
      'type' => 'redhen_contact',
      'description' => t("The redhen_contact this engagement applies to."),
      'schema field' => 'contact_id',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the note."),
      'schema field' => 'author_uid',
    );

    $properties['engagement_score'] = array(
      'label' => t('Engagement score'),
      'type' => 'redhen_engagement_score',
      'schema field' => 'engagement_score',
      'description' => t('The engagement score.'),
      'options list' => 'redhen_engagement_score_options',
      'setter callback' => 'entity_property_verbatim_set',
      // RedHen specific attributes
      'field_type' => 'select',
      'filter' => TRUE,
    );

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

/**
 * Callback to get $engagement->entity.
 *
 * @see redhen_engagement_entity_property_info().
 */
function redhen_engagement_property_source_get($engagement, array $options, $property_name, $entity_type) {
  $entity = entity_load_single($engagement->entity_type, $engagement->entity_id);
  return ($entity) ? entity_metadata_wrapper($engagement->entity_type, $entity): NULL;
}
